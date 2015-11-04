#!/usr/bin/python

import sqlite3

def main():

    conn = sqlite3.connect('PadmeDAQ.db');
    c = conn.cursor()

    c.execute('DROP TABLE IF EXISTS replica')
    c.execute('DROP TABLE IF EXISTS file')
    c.execute('DROP TABLE IF EXISTS proc_config_para')
    c.execute('DROP TABLE IF EXISTS process')
    c.execute('DROP TABLE IF EXISTS channel')
    c.execute('DROP TABLE IF EXISTS board')
    c.execute('DROP TABLE IF EXISTS run_config_para')
    c.execute('DROP TABLE IF EXISTS board_type')
    c.execute('DROP TABLE IF EXISTS run')
    c.execute('DROP TABLE IF EXISTS config_para_name')

    c.execute('''
CREATE TABLE run (
  number        INTEGER PRIMARY KEY,
  type          TEXT,
  status        INTEGER,
  time_start    INTEGER,
  time_stop     INTEGER,
  total_events  INTEGER,
  comment       TEXT
)
''')

    c.execute('''
CREATE TABLE file (
  id       INTEGER PRIMARY KEY,
  name     TEXT UNIQUE,
  type     TEXT,
  n_events INTEGER,
  size     INTEGER
)
''')

    c.execute('''
CREATE TABLE config_para_name(
  id   INTEGER PRIMARY KEY,
  name TEXT UNIQUE
)
''')

    c.execute('''
CREATE TABLE board_type(
  id           INTEGER PRIMARY KEY,
  type         TEXT,
  manufacturer TEXT,
  model        TEXT UNIQUE,
  n_channels   INTEGER,
  UNIQUE(manufacturer,model)
)
''')

    c.execute('''
CREATE TABLE run_config_para(
  id                  INTEGER PRIMARY KEY,
  run_number          INTEGER,
  config_para_name_id INTEGER,
  value               TEXT,
  FOREIGN KEY(run_number) REFERENCES run(number),
  FOREIGN KEY(config_para_name_id) REFERENCES config_para_name(id)
)
''')

    c.execute('''
CREATE TABLE replica (
  id         INTEGER PRIMARY KEY,
  file_id    INTEGER,
  url        TEXT UNIQUE,
  status     INTEGER,
  FOREIGN KEY(file_id) REFERENCES file(id)
)
''')

    c.execute('''
CREATE TABLE board(
  id            INTEGER PRIMARY KEY,
  board_type_id INTEGER,
  serial_number TEXT,
  UNIQUE(board_type_id,serial_number),
  FOREIGN KEY(board_type_id) REFERENCES board_type(id)
)
''')

    c.execute('''
CREATE TABLE run_file (
  run_number INTEGER,
  file_id    INTEGER,
  PRIMARY KEY(run_number,file_id),
  FOREIGN KEY(run_number) REFERENCES run(number),
  FOREIGN KEY(file_id) REFERENCES file(id)
)
''')

    c.execute('''
CREATE TABLE channel(
  id       INTEGER PRIMARY KEY,
  board_id INTEGER,
  FOREIGN KEY(board_id) REFERENCES board(id)
)
''')

    c.execute('''
CREATE TABLE process (
  id           INTEGER PRIMARY KEY,
  run_number   INTEGER,
  board_id     INTEGER,
  status       INTEGER,
  time_start   INTEGER,
  time_stop    INTEGER,
  n_raw_files  INTEGER,
  total_events INTEGER,
  total_size   INTEGER,
  UNIQUE(run_number,board_id),
  FOREIGN KEY(run_number) REFERENCES run(number),
  FOREIGN KEY(board_id) REFERENCES board(id)
)
''')

    c.execute('''
CREATE TABLE raw_file (
  id         INTEGER PRIMARY KEY,
  name       TEXT UNIQUE,
  version    INTEGER,
  process_id INTEGER,
  part       INTEGER,
  time_open  INTEGER,
  time_close INTEGER,
  n_events   INTEGER,
  size       INTEGER,
  FOREIGN KEY(process_id) REFERENCES process(id)
)
''')

    c.execute('''
CREATE TABLE proc_config_para(
  id                  INTEGER PRIMARY KEY,
  process_id          INTEGER,
  config_para_name_id INTEGER,
  value               TEXT,
  FOREIGN KEY(process_id) REFERENCES process(id),
  FOREIGN KEY(config_para_name_id) REFERENCES config_para_name(id)
)
''')

#print
#c.execute('PRAGMA table_info(sqlite_master)')
#for r in c.fetchall():
#    (id,name,type,canbenull,default,isprimarykey) = r
#    print id,name,type,canbenull,default,isprimarykey

    # Show all tables
    c.execute('SELECT name FROM sqlite_master WHERE type="table"')
    res1 = c.fetchall()
    for r1 in res1:
        (tab,) = r1
        print "\nTable",tab
        c.execute('PRAGMA table_info(%s)' % tab)
        res2 = c.fetchall()
        for r2 in res2:
            (id,name,type,canbenull,default,isprimarykey) = r2
            print id,name,type,canbenull,default,isprimarykey

    conn.commit()
    conn.close()

# Execution starts here
if __name__ == "__main__":
   main()
