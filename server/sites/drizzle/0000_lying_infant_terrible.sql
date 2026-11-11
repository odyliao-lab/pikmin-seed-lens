CREATE TABLE `contacts` (
	`id` text PRIMARY KEY NOT NULL,
	`email` text NOT NULL,
	FOREIGN KEY (`id`) REFERENCES `reports`(`id`) ON UPDATE no action ON DELETE cascade
);
--> statement-breakpoint
CREATE TABLE `rates` (
	`key` text PRIMARY KEY NOT NULL,
	`count` integer NOT NULL,
	`until` integer NOT NULL
);
--> statement-breakpoint
CREATE TABLE `reports` (
	`id` text PRIMARY KEY NOT NULL,
	`payload` text NOT NULL,
	`digest` text NOT NULL,
	`created` integer NOT NULL,
	`state` text DEFAULT 'new' NOT NULL
);
