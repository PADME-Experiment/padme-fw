-- MySQL Script generated by MySQL Workbench
-- 11/29/19 08:14:04
-- Model: New Model    Version: 1.0
-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema PadmeDAQ
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `PadmeDAQ` ;

-- -----------------------------------------------------
-- Schema PadmeDAQ
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `PadmeDAQ` DEFAULT CHARACTER SET utf8 ;
USE `PadmeDAQ` ;

-- -----------------------------------------------------
-- Table `PadmeDAQ`.`run_type`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`run_type` (
  `id` INT NOT NULL,
  `type` VARCHAR(255) NOT NULL,
  `description` TEXT NULL,
  UNIQUE INDEX `short_name_UNIQUE` (`type` ASC),
  PRIMARY KEY (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`run`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`run` (
  `number` INT NOT NULL,
  `name` VARCHAR(255) NULL,
  `user` VARCHAR(1024) NULL,
  `run_type_id` INT NOT NULL,
  `status` INT NOT NULL,
  `time_init` DATETIME NULL,
  `time_start` DATETIME NULL,
  `time_stop` DATETIME NULL,
  `total_events` INT UNSIGNED NULL,
  PRIMARY KEY (`number`),
  INDEX `fk_run_run_type1_idx` (`run_type_id` ASC),
  UNIQUE INDEX `name_UNIQUE` (`name` ASC),
  CONSTRAINT `fk_run_run_type1`
    FOREIGN KEY (`run_type_id`)
    REFERENCES `PadmeDAQ`.`run_type` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`board_type`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`board_type` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `type` VARCHAR(45) NOT NULL,
  `manufacturer` VARCHAR(45) NOT NULL,
  `model` VARCHAR(45) NOT NULL,
  `n_channels` INT UNSIGNED NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `manu_model` (`manufacturer` ASC, `model` ASC))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`board`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`board` (
  `id` INT NOT NULL,
  `serial_number` VARCHAR(45) NOT NULL,
  `board_type_id` INT NOT NULL,
  PRIMARY KEY (`id`),
  INDEX `fk_board_board_type1_idx` (`board_type_id` ASC),
  UNIQUE INDEX `u_btsn` (`serial_number` ASC, `board_type_id` ASC),
  CONSTRAINT `fk_board_board_type1`
    FOREIGN KEY (`board_type_id`)
    REFERENCES `PadmeDAQ`.`board_type` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`config_para_name`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`config_para_name` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(128) NOT NULL,
  `description` TEXT NULL,
  UNIQUE INDEX `name_UNIQUE` (`name` ASC),
  PRIMARY KEY (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`run_config_para`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`run_config_para` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `run_number` INT NOT NULL,
  `config_para_name_id` INT NOT NULL,
  `value` VARCHAR(1024) NOT NULL,
  PRIMARY KEY (`id`),
  INDEX `fk_run_config_para_config_para_name1_idx` (`config_para_name_id` ASC),
  INDEX `fk_run_config_para_run1_idx` (`run_number` ASC),
  CONSTRAINT `fk_run_config_para_config_para_name1`
    FOREIGN KEY (`config_para_name_id`)
    REFERENCES `PadmeDAQ`.`config_para_name` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_run_config_para_run1`
    FOREIGN KEY (`run_number`)
    REFERENCES `PadmeDAQ`.`run` (`number`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`node`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`node` (
  `id` INT NOT NULL,
  `name` VARCHAR(255) NOT NULL,
  `mac_addr_lnf` CHAR(17) NULL,
  `ip_addr_lnf` VARCHAR(15) NULL,
  `mac_addr_daq` CHAR(17) NULL,
  `ip_addr_daq` VARCHAR(15) NULL,
  `mac_addr_dcs` CHAR(17) NULL,
  `ip_addr_dcs` VARCHAR(15) NULL,
  `mac_addr_ipmi` CHAR(17) NULL,
  `ip_addr_ipmi` VARCHAR(15) NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `name_UNIQUE` (`name` ASC),
  UNIQUE INDEX `mac_addr_lnf_UNIQUE` (`mac_addr_lnf` ASC),
  UNIQUE INDEX `ip_addr_lnf_UNIQUE` (`ip_addr_lnf` ASC),
  UNIQUE INDEX `mac_addr_daq_UNIQUE` (`mac_addr_daq` ASC),
  UNIQUE INDEX `ip_addr_daq_UNIQUE` (`ip_addr_daq` ASC),
  UNIQUE INDEX `mac_addr_dcs_UNIQUE` (`mac_addr_dcs` ASC),
  UNIQUE INDEX `ip_addr_dcs_UNIQUE` (`ip_addr_dcs` ASC),
  UNIQUE INDEX `mac_addr_ipmi_UNIQUE` (`mac_addr_ipmi` ASC),
  UNIQUE INDEX `ip_addr_ipmi_UNIQUE` (`ip_addr_ipmi` ASC))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`optical_link`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`optical_link` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `node_id` INT NOT NULL,
  `controller_id` INT UNSIGNED NOT NULL,
  `channel_id` INT UNSIGNED NOT NULL,
  `slot_id` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`id`),
  INDEX `fk_optical_link_node1_idx` (`node_id` ASC),
  CONSTRAINT `fk_optical_link_node1`
    FOREIGN KEY (`node_id`)
    REFERENCES `PadmeDAQ`.`node` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`l_board_optical_link`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`l_board_optical_link` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `board_id` INT NOT NULL,
  `optical_link_id` INT NOT NULL,
  `time_start` DATETIME NULL,
  `time_stop` DATETIME NULL,
  INDEX `fk_l_board_optical_link_board1_idx` (`board_id` ASC),
  INDEX `fk_l_board_optical_link_optical_link1_idx` (`optical_link_id` ASC),
  PRIMARY KEY (`id`),
  CONSTRAINT `fk_l_board_optical_link_board1`
    FOREIGN KEY (`board_id`)
    REFERENCES `PadmeDAQ`.`board` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_l_board_optical_link_optical_link1`
    FOREIGN KEY (`optical_link_id`)
    REFERENCES `PadmeDAQ`.`optical_link` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`log_entry`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`log_entry` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `run_number` INT NOT NULL,
  `type` VARCHAR(128) NULL,
  `level` INT NULL,
  `time` DATETIME NULL,
  `text` TEXT NULL,
  INDEX `fk_log_entry_run1_idx` (`run_number` ASC),
  PRIMARY KEY (`id`),
  CONSTRAINT `fk_log_entry_run1`
    FOREIGN KEY (`run_number`)
    REFERENCES `PadmeDAQ`.`run` (`number`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`process_type`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`process_type` (
  `id` INT NOT NULL,
  `type` VARCHAR(255) NOT NULL,
  `executable` VARCHAR(255) NULL,
  `description` TEXT NULL,
  UNIQUE INDEX `short_name_UNIQUE` (`type` ASC),
  PRIMARY KEY (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`process`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`process` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `run_number` INT NOT NULL,
  `process_type_id` INT NOT NULL,
  `node_id` INT NOT NULL,
  `status` INT NOT NULL,
  `time_create` DATETIME NULL,
  `time_start` DATETIME NULL,
  `time_stop` DATETIME NULL,
  `time_end` DATETIME NULL,
  `n_files` INT UNSIGNED NULL,
  `total_events` INT UNSIGNED NULL,
  `total_size` BIGINT UNSIGNED NULL,
  PRIMARY KEY (`id`),
  INDEX `fk_daq_process_run1_idx` (`run_number` ASC),
  INDEX `fk_process_node1_idx` (`node_id` ASC),
  INDEX `fk_process_process_type1_idx` (`process_type_id` ASC),
  CONSTRAINT `fk_daq_process_run1`
    FOREIGN KEY (`run_number`)
    REFERENCES `PadmeDAQ`.`run` (`number`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_process_node1`
    FOREIGN KEY (`node_id`)
    REFERENCES `PadmeDAQ`.`node` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_process_process_type1`
    FOREIGN KEY (`process_type_id`)
    REFERENCES `PadmeDAQ`.`process_type` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`daq_link`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`daq_link` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `process_id` INT NOT NULL,
  `optical_link_id` INT NOT NULL,
  PRIMARY KEY (`id`),
  INDEX `fk_daq_process_optical_link1_idx` (`optical_link_id` ASC),
  INDEX `fk_daq_link_process1_idx` (`process_id` ASC),
  CONSTRAINT `fk_daq_process_optical_link1`
    FOREIGN KEY (`optical_link_id`)
    REFERENCES `PadmeDAQ`.`optical_link` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_daq_link_process1`
    FOREIGN KEY (`process_id`)
    REFERENCES `PadmeDAQ`.`process` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`file_type`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`file_type` (
  `id` INT NOT NULL,
  `type` VARCHAR(255) NOT NULL,
  `producer` VARCHAR(255) NULL,
  `description` TEXT NULL,
  UNIQUE INDEX `short_name_UNIQUE` (`type` ASC),
  PRIMARY KEY (`id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`file`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`file` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(255) NOT NULL,
  `file_type_id` INT NOT NULL,
  `version` INT NULL,
  `process_id` INT NOT NULL,
  `part` INT UNSIGNED NULL,
  `status` INT NULL,
  `time_open` DATETIME NULL,
  `time_close` DATETIME NULL,
  `n_events` INT UNSIGNED NULL,
  `size` BIGINT UNSIGNED NULL,
  `adler32` CHAR(8) NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `raw_file_name_UNIQUE` (`name` ASC),
  INDEX `fk_file_process1_idx` (`process_id` ASC),
  INDEX `fk_file_file_type1_idx` (`file_type_id` ASC),
  CONSTRAINT `fk_file_process1`
    FOREIGN KEY (`process_id`)
    REFERENCES `PadmeDAQ`.`process` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_file_file_type1`
    FOREIGN KEY (`file_type_id`)
    REFERENCES `PadmeDAQ`.`file_type` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`proc_config_para`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`proc_config_para` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `process_id` INT NOT NULL,
  `config_para_name_id` INT NOT NULL,
  `value` VARCHAR(1024) NOT NULL,
  PRIMARY KEY (`id`),
  INDEX `fk_proc_config_para_process1_idx` (`process_id` ASC),
  INDEX `fk_proc_config_para_config_para_name1_idx` (`config_para_name_id` ASC),
  CONSTRAINT `fk_proc_config_para_process1`
    FOREIGN KEY (`process_id`)
    REFERENCES `PadmeDAQ`.`process` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_proc_config_para_config_para_name1`
    FOREIGN KEY (`config_para_name_id`)
    REFERENCES `PadmeDAQ`.`config_para_name` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PadmeDAQ`.`proc_log_entry`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PadmeDAQ`.`proc_log_entry` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `process_id` INT NOT NULL,
  `type` VARCHAR(128) NULL,
  `level` INT NULL,
  `time` DATETIME NULL,
  `text` TEXT NULL,
  PRIMARY KEY (`id`),
  INDEX `fk_proc_log_entry_process1_idx` (`process_id` ASC),
  CONSTRAINT `fk_proc_log_entry_process1`
    FOREIGN KEY (`process_id`)
    REFERENCES `PadmeDAQ`.`process` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
