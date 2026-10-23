-- ScenicFlow database schema (SQLite)
-- Field names are snake_case in DB; controllers map them to the camelCase JSON the frontend expects.

CREATE TABLE IF NOT EXISTS scenic_spot (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    spot_id TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    instant_capacity INTEGER NOT NULL,
    daily_capacity INTEGER NOT NULL,
    current_visitors INTEGER NOT NULL DEFAULT 0,
    cumulative_visitors INTEGER NOT NULL DEFAULT 0,
    queue_minutes INTEGER NOT NULL DEFAULT 0,      -- user-side estimated queue time
    open_time TEXT NOT NULL DEFAULT '08:00',       -- user-side spot detail
    close_time TEXT NOT NULL DEFAULT '18:00',
    location_x REAL NOT NULL DEFAULT 0,
    location_y REAL NOT NULL DEFAULT 0,
    latitude REAL NOT NULL DEFAULT 0,              -- real-world lat for map rendering
    longitude REAL NOT NULL DEFAULT 0,             -- real-world lng for map rendering
    arrival_rate REAL NOT NULL DEFAULT 0,
    departure_rate REAL NOT NULL DEFAULT 0,
    status INTEGER NOT NULL DEFAULT 1,
    created_at TEXT DEFAULT (datetime('now','localtime')),
    updated_at TEXT DEFAULT (datetime('now','localtime'))
);

CREATE TABLE IF NOT EXISTS parking_lot (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    parking_id TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    total_spaces INTEGER NOT NULL,
    occupied_spaces INTEGER NOT NULL DEFAULT 0,
    distance_m INTEGER NOT NULL DEFAULT 0,         -- walking distance from main gate (user side)
    latitude REAL NOT NULL DEFAULT 0,              -- for "navigate" deep links
    longitude REAL NOT NULL DEFAULT 0,
    status INTEGER NOT NULL DEFAULT 1
);

CREATE TABLE IF NOT EXISTS shuttle_bus (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    shuttle_id TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    capacity INTEGER NOT NULL,
    current_station TEXT,
    current_passengers INTEGER NOT NULL DEFAULT 0,
    status TEXT NOT NULL DEFAULT 'IDLE'   -- RUNNING / IDLE / MAINTENANCE
);

CREATE TABLE IF NOT EXISTS shuttle_route (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    route_name TEXT NOT NULL DEFAULT '',           -- user side groups rows by route
    from_station TEXT NOT NULL,
    to_station TEXT NOT NULL,
    distance INTEGER NOT NULL,                     -- meters
    waiting_minutes INTEGER NOT NULL DEFAULT 0,    -- average wait per route (user side)
    status INTEGER NOT NULL DEFAULT 1
);
CREATE INDEX IF NOT EXISTS idx_route_name ON shuttle_route(route_name);

CREATE TABLE IF NOT EXISTS flow_record (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    record_time TEXT NOT NULL,
    spot_id TEXT NOT NULL,
    current_visitors INTEGER NOT NULL,
    cumulative_visitors INTEGER NOT NULL,
    congestion_level TEXT NOT NULL,
    arrival_rate REAL DEFAULT 0,
    departure_rate REAL DEFAULT 0,
    is_deleted INTEGER NOT NULL DEFAULT 0
);
CREATE INDEX IF NOT EXISTS idx_flow_spot_time ON flow_record(spot_id, record_time);

CREATE TABLE IF NOT EXISTS event (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_type TEXT NOT NULL,
    occur_time TEXT NOT NULL,
    spot_id TEXT,
    level TEXT NOT NULL,
    description TEXT,
    suggestion TEXT,
    status TEXT NOT NULL DEFAULT 'ACTIVE',
    updated_at TEXT
);
CREATE INDEX IF NOT EXISTS idx_event_type_time ON event(event_type, occur_time);

CREATE TABLE IF NOT EXISTS scheduling_record (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    task_id TEXT NOT NULL,
    spot_id TEXT,
    schedule_type TEXT NOT NULL,
    content TEXT,
    detail TEXT,
    suggestion_status TEXT NOT NULL DEFAULT 'PENDING',  -- PENDING / EXECUTED / REJECTED
    create_time TEXT NOT NULL,
    is_deleted INTEGER NOT NULL DEFAULT 0
);
CREATE INDEX IF NOT EXISTS idx_scheduling_time ON scheduling_record(create_time);
CREATE INDEX IF NOT EXISTS idx_scheduling_type ON scheduling_record(schedule_type, suggestion_status);

CREATE TABLE IF NOT EXISTS rule (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    spot_id TEXT NOT NULL DEFAULT '*',
    metric TEXT NOT NULL,
    operator TEXT NOT NULL,
    threshold REAL NOT NULL,
    action TEXT NOT NULL,
    level TEXT NOT NULL DEFAULT 'WARNING',
    priority INTEGER NOT NULL DEFAULT 50,
    status INTEGER NOT NULL DEFAULT 1
);

CREATE TABLE IF NOT EXISTS user (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    role TEXT NOT NULL,                  -- admin / operator / viewer
    created_at TEXT DEFAULT (datetime('now','localtime'))
);

CREATE TABLE IF NOT EXISTS system_config (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS auth_token (
    token TEXT PRIMARY KEY,
    username TEXT NOT NULL,
    role TEXT NOT NULL,
    expire_time TEXT NOT NULL
);

-- User-side POI navigation (toilets / parking lots / stations).
CREATE TABLE IF NOT EXISTS poi (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    poi_id TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    category TEXT NOT NULL,               -- TOILET / PARKING / STATION / SERVICE / MEDICAL
    location_x REAL NOT NULL DEFAULT 0,   -- scenic-area map coordinates
    location_y REAL NOT NULL DEFAULT 0,
    latitude REAL NOT NULL DEFAULT 0,     -- for "navigate" deep links
    longitude REAL NOT NULL DEFAULT 0,
    description TEXT,
    status INTEGER NOT NULL DEFAULT 1
);
CREATE INDEX IF NOT EXISTS idx_poi_category ON poi(category);

-- Per-spot nearby facilities with walking distance (spot detail page).
CREATE TABLE IF NOT EXISTS spot_poi (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    spot_id TEXT NOT NULL,
    poi_id TEXT NOT NULL,
    distance_m INTEGER NOT NULL
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_spot_poi ON spot_poi(spot_id, poi_id);

-- Hourly visitor forecast for the whole park (user-side home page).
CREATE TABLE IF NOT EXISTS flow_forecast (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    forecast_date TEXT NOT NULL,          -- YYYY-MM-DD
    hour INTEGER NOT NULL,                -- 0..23
    predicted_visitors INTEGER NOT NULL,
    predicted_level TEXT NOT NULL,        -- congestion level of the whole park
    created_at TEXT DEFAULT (datetime('now','localtime'))
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_forecast_date_hour ON flow_forecast(forecast_date, hour);
