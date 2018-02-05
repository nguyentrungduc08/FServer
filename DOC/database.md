# script create database:

create database 
```
CREATE DATABASE FILE;
```

change database is using
```
USE FILE
```

create table users

```
CREATE TABLE USERS (
id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, 
username VARCHAR(50),
password VARCHAR(50),
create_date DATE,
last_login DATE,
status TINYINT NOT NULL DEFAULT 0
);
```

insert data for USERS table
```
INSERT INTO `USERS` (`username`,`password`,`create_date`,`last_login`) VALUES ("user1", "user1", '2018-02-5','2018-02-5');
INSERT INTO `USERS` (`username`,`password`,`create_date`,`last_login`) VALUES ("user2", "user2", '2018-02-5','2018-02-5');
INSERT INTO `USERS` (`username`,`password`,`create_date`,`last_login`) VALUES ("user3", "user3", '2018-02-5','2018-02-5');
INSERT INTO `USERS` (`username`,`password`,`create_date`,`last_login`) VALUES ("user4", "user4", '2018-02-5','2018-02-5');
```

create table filesB for backup file

```
CREATE TABLE FILESB(
id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
owner VARCHAR(50) NOT NULL,
file_url TEXT NOT NULL,
file_name VARCHAR(250) NOT NULL,
checksum VARCHAR(256) NOT NULL,
file_length INT,
upload_time DATE,
file_status TINYINT NOT NULL DEFAULT 0
);
```


create table filesT for transfer file

```
CREATE TABLE FILEST(
id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
sender VARCHAR(50) NOT NULL,
receiver VARCHAR(50) NOT NULL,
file_url TEXT NOT NULL,
file_name VARCHAR(250) NOT NULL,
checksum VARCHAR(256) NOT NULL,
file_length INT,
upload_time DATE,
file_status TINYINT NOT NULL DEFAULT 0
);
```

create user 
```
CREATE USER testuser@localhost IDENTIFIED BY 'testuser';
```

grant all priviliges to testuser on FILE database
```
GRANT ALL ON FILE.* to testuser@localhost;
```