import { sqliteTable, text, integer } from 'drizzle-orm/sqlite-core';
export const reports = sqliteTable('reports', {
 id: text('id').primaryKey(), payload: text('payload').notNull(), digest: text('digest').notNull(),
 created: integer('created').notNull(), state: text('state').notNull().default('new'),
});
export const contacts = sqliteTable('contacts', {
 id: text('id').primaryKey().references(()=>reports.id,{onDelete:'cascade'}), email: text('email').notNull(),
});
export const rates = sqliteTable('rates', { key: text('key').primaryKey(), count: integer('count').notNull(), until: integer('until').notNull() });
