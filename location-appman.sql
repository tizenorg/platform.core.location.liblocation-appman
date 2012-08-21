PRAGMA journal_mode = PERSIST;

DROP TABLE IF EXISTS "LocationApp";
CREATE TABLE LocationApp(
	package		VARCHAR(64) PRIMARY KEY,
	icon		VARCHAR(256),
	app_id		INTEGER,
	installed_date	INTEGER,
	recently_used	INTEGER,
	enable		BOOL
	);

INSERT INTO "LocationApp" (package, app_id, installed_date, recently_used, enable) VALUES ("HomeScreen", 0, 0, 0, 0);
INSERT INTO "LocationApp" (package, app_id, installed_date, recently_used, enable) VALUES ("com.samsung.setting", 0, 0, 0, 1);

