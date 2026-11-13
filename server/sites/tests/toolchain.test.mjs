import { test } from 'node:test';
import assert from 'node:assert/strict';
import { mkdtemp, mkdir, cp, readFile, writeFile, rm } from 'node:fs/promises';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import path from 'node:path';
import { createRequire } from 'node:module';
import { parse } from '@typescript-eslint/typescript-estree';

const root = fileURLToPath(new URL('../', import.meta.url));
const require = createRequire(import.meta.url);

test('patched Satori compression dependency retains ZIP decoding and SVG rendering', async () => {
  const satoriRequire = createRequire(require.resolve('satori'));
  const { zipSync, unzipSync, strToU8, strFromU8 } = satoriRequire('fflate');
  assert.equal(strFromU8(unzipSync(zipSync({ 'fixture.txt': strToU8('synthetic fixture') }))['fixture.txt']), 'synthetic fixture');
  const { default: satori } = await import('satori');
  const svg = await satori({ type: 'div', props: { style: { width: 80, height: 40, backgroundColor: 'green' } } }, { width: 80, height: 40, fonts: [] });
  assert.match(svg, /<svg/);
});

test('scoped minimatch upgrade retains TypeScript parsing and ESLint file selection', async () => {
  const nestedRequire = createRequire(require.resolve('@typescript-eslint/typescript-estree'));
  const { minimatch } = nestedRequire('minimatch');
  const options = { optimizationLevel: 2 };
  assert.equal(minimatch('app/api/submit/route.ts', '**/*.{ts,tsx}', options), true);
  assert.equal(minimatch('app/api/submit/route.js', '**/*.{ts,tsx}', options), false);
  assert.equal(minimatch('.private/secret.ts', '**/*.ts', options), false);
  const ast = parse('export const result: number = 1;', { sourceType: 'module' });
  assert.equal(ast.body[0].type, 'ExportNamedDeclaration');
  const { ESLint } = await import('eslint');
  const lint = new ESLint({ cwd: root });
  const config = await lint.calculateConfigForFile('app/api/submit/route.ts');
  assert.ok(config.languageOptions.parser);
  const results = await lint.lintText('export const value: number = 1;\n', { filePath: 'app/security-fixture.ts' });
  assert.deepEqual(results.flatMap(r => r.messages), []);
});

test('patched legacy esbuild loader generates migrations without rewriting existing history', async () => {
  // Exercise the actual CLI and TypeScript config/schema loader in an isolated copy.
  await mkdir(path.join(root, '.sites-runtime'), { recursive: true });
  const tmp = await mkdtemp(path.join(root, '.sites-runtime/migration-'));
  try {
    await cp(path.join(root, 'drizzle'), path.join(tmp, 'drizzle'), { recursive: true });
    await cp(path.join(root, 'db'), path.join(tmp, 'db'), { recursive: true });
    await cp(path.join(root, 'drizzle.config.ts'), path.join(tmp, 'drizzle.config.ts'));
    const migration = 'drizzle/0000_lying_infant_terrible.sql';
    const before = await readFile(path.join(tmp, migration), 'utf8');
    const schema = path.join(tmp, 'db/schema.ts');
    await writeFile(schema, (await readFile(schema, 'utf8')) + '\nexport const securityFixture = sqliteTable("security_fixture", { id: text("id").primaryKey() });\n');
    const result = spawnSync(process.execPath, [path.join(root, 'node_modules/drizzle-kit/bin.cjs'), 'generate'], { cwd: tmp, encoding: 'utf8', timeout: 60_000 });
    assert.equal(result.status, 0, result.stdout + result.stderr);
    assert.equal(await readFile(path.join(tmp, migration), 'utf8'), before);
    const { readdir } = await import('node:fs/promises');
    const generated = (await readdir(path.join(tmp, 'drizzle'))).filter(n => n.endsWith('.sql') && !migration.endsWith(n));
    assert.equal(generated.length, 1, result.stdout);
    assert.match(await readFile(path.join(tmp, 'drizzle', generated[0]), 'utf8'), /CREATE TABLE `security_fixture`/);
  } finally {
    await rm(tmp, { recursive: true, force: true });
  }
});
