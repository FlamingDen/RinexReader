-- скрипт для создания проверосчной бд для facadeDB
-- и проверки ее выгрзки и чтения rinex файлов в БД

create extension timescaledb;
--create extension postgis;

--типы обозреваемых данных для каждой ГНСС
create table gnss_obs_type(
	id_gnss character(2) not null,
	obs_types character(3)[16],
	constraint pk_id_gnss primary key(id_gnss)
);

--спутники
create table public.satellites(
	id_satellite serial,
	prn varchar(8) primary key
); 

--данные(это и будет гипертаблица)
create table data_obs(
	date_time  timestamp without time zone,
	prn varchar(8) references satellites(prn),
	gnss character(2) references gnss_obs_type(id_gnss),
	obs_data numeric(16,6)[16]
);


insert into public.gnss_obs_type
values 	('G','{"C1C", "L1C", "D1C", "S1C", "C2S", "L2S", "D2S", "S2S", "C2W", "L2W", "D2W", "S2W", "C5Q", "L5Q", "D5Q", "S5Q"}'),
		('R','{"C1C", "L1C", "D1C", "S1C", "C2P", "L2P", "D2P", "S2P", "C2C", "L2C", "D2C", "S2C"}'),
		('E','{"C1C", "L1C", "D1C", "S1C", "C5Q", "L5Q", "D5Q", "S5Q", "C7Q", "L7Q", "D7Q", "S7Q", "C8Q", "L8Q", "D8Q", "S8Q"}'),
		('C','{"C2I", "L2I", "D2I", "S2I", "C6I", "L6I", "D6I", "S6I", "C7I", "L7I", "D7I", "S7I"}');
	
select create_hypertable('data_obs',by_range('date_time'));

--drop table data_obs ;
--drop table gnss_obs_type ;
--drop table satellites ;
--
--delete from data_obs;
--delete from satellites; 
--delete from gnss_obs_type ;
--
--select count(*) from data_obs;
--select count(*) from satellites s;
--select count(*) from gnss_obs_type got ;
--
--select * from gnss_obs_type;	
--select * from data_obs;	
--select * from public.satellites s ;





