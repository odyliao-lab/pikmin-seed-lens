import assert from "node:assert/strict";
import test from "node:test";
import {
  invocationSchema, grantSchema, sessionOptionsSchema, driverMessageSchema,
} from "../scripts/connector-preview/protocol.mjs";
import { createPreviewBinding } from "../scripts/connector-preview/host-binding.mjs";

const id = "01234567-89ab-4cde-8fab-0123456789ab";
const call = {
  connectorId: "synthetic", actionName: "read",
  arguments: { query: "fixture", nested: { items: [1, true, null] } },
};
const schemas = { invocationSchema, grantSchema, sessionOptionsSchema, driverMessageSchema };
const cases = [
  ["nested invocation", "invocationSchema", call, true],
  ["empty argument object", "invocationSchema", { ...call, arguments: {} }, true],
  ["unexpected account field", "invocationSchema", { ...call, account: "unexpected" }, false],
  ["array arguments", "invocationSchema", { ...call, arguments: [] }, false],
  ["invalid connector name", "invocationSchema", { ...call, connectorId: "bad/name" }, false],
  ["read-only grant", "grantSchema", { connectorId: "synthetic", actionName: "read", readOnly: true }, true],
  ["write grant", "grantSchema", { connectorId: "synthetic", actionName: "read", readOnly: false }, false],
  ["default options", "sessionOptionsSchema", {}, true],
  ["positive limits", "sessionOptionsSchema", { timeoutMs: 1, lifetimeMs: 1, maxCalls: 1, maxConcurrent: 1 }, true],
  ["zero call limit", "sessionOptionsSchema", { maxCalls: 0 }, false],
  ["timer overflow", "sessionOptionsSchema", { timeoutMs: 2 ** 31 }, false],
  ["driver result record", "driverMessageSchema", { jsonrpc: "2.0", id, result: { content: [] } }, true],
  ["driver stop", "driverMessageSchema", { jsonrpc: "2.0", method: "stop" }, true],
  ["driver error", "driverMessageSchema", { jsonrpc: "2.0", id, error: { code: -1, message: "fixture" } }, true],
  ["invalid driver UUID", "driverMessageSchema", { jsonrpc: "2.0", id: "bad", result: {} }, false],
  ["unexpected driver field", "driverMessageSchema", { jsonrpc: "2.0", id, result: {}, extra: true }, false],
];
for (const [name, schema, data, expected] of cases) {
  test(name, () => assert.equal(schemas[schema].safeParse(data).success, expected));
}

const request = (data) => new Request("http://127.0.0.1/invoke", {
  method: "POST", body: JSON.stringify(data),
});
const tool = (invoke) => ({ connectorId: "synthetic", actionName: "read", readOnly: true, invoke });
const result = { content: [{ type: "text", text: "fixture" }] };

test("defaults remain bounded and invalid grants fail before invocation", () => {
  assert.deepEqual(sessionOptionsSchema.parse({}), { timeoutMs: 120000, maxConcurrent: 64 });
  const read = tool(async () => result);
  assert.throws(() => createPreviewBinding({ tools: [{ ...read, readOnly: false }] }));
  assert.throws(() => createPreviewBinding({ tools: [read, read] }));
});

test("invalid envelopes, oversized bodies and ungranted actions never invoke", async (t) => {
  let invoked = 0;
  const binding = createPreviewBinding({ tools: [tool(async () => { invoked++; return result; })] });
  t.after(() => binding.close());
  for (const body of [{ ...call, account: "unexpected" }, { ...call, arguments: { text: "x".repeat(65536) } }]) {
    assert.equal((await (await binding.fetch(request(body))).json()).status, "invalid_request");
  }
  assert.equal((await (await binding.fetch(request({ ...call, actionName: "other" }))).json()).status, "tool_not_allowed");
  assert.equal(invoked, 0);
});

test("real binding preserves arguments and enforces call quota and closure", async (t) => {
  let invoked = 0;
  const binding = createPreviewBinding({ maxCalls: 1, tools: [tool(async (args) => {
    invoked++;
    assert.deepEqual(args, call.arguments);
    return result;
  })] });
  t.after(() => binding.close());
  assert.deepEqual(await (await binding.fetch(request(call))).json(), { status: "success", result });
  assert.equal((await (await binding.fetch(request(call))).json()).status, "tool_not_allowed");
  binding.close();
  assert.equal((await (await binding.fetch(request(call))).json()).status, "binding_unavailable");
  assert.equal(invoked, 1);
});

test("provider-authored transport status is refused", async (t) => {
  const binding = createPreviewBinding({ tools: [tool(async () => ({ ...result, status: "success" }))] });
  t.after(() => binding.close());
  assert.equal((await (await binding.fetch(request(call))).json()).status, "upstream_error");
});

test("close interrupts an active wait without replaying the provider", async (t) => {
  let release, started;
  const ready = new Promise((resolve) => { started = resolve; });
  const pending = new Promise((resolve) => { release = resolve; });
  let invoked = 0;
  const binding = createPreviewBinding({ tools: [tool(async () => { invoked++; started(); return pending; })] });
  t.after(() => { binding.close(); release(result); });
  const reply = binding.fetch(request(call));
  await ready;
  binding.close();
  assert.equal((await (await reply).json()).status, "upstream_error");
  release(result);
  assert.equal((await (await binding.fetch(request(call))).json()).status, "binding_unavailable");
  assert.equal(invoked, 1);
});
