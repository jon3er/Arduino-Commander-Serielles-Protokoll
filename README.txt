----------------------------------------------------------------------------------
-- Auftraggeber:		HTW Dresden - Elektrotechnik und Informationstechnik - Projektarbeit
--						      Prof. Dr.-Ing. habil. Göhler
--
-- Entwickler:			Jonathan Dreyer, Maximilian Stober
-- 
-- Projekt:				  Serielles Protokoll Arduino Commander 
--
-- Beschreibung:		Skript zum UART-Kommunikationsaufbau mit United Automation - FC36MV Three Phase Firing Board (B6C)
--						      und Darstellung übertragener Daten per LCD-Modul
--
--	Kommentar:			
--
-- Letzte Änderung:	11.01.2025   J. Dreyer
----------------------------------------------------------------------------------


-- OrderStruktur


├── .pio							# PlatformIO settings	
├── .vscode							# vscode projekt settings
├── include							# Header files
|	├── uart_com.h					# uart function defines
|   ├── e_uart_msg.h				# uart enums
|   ├── c_uart_com.h				# uart communication class
|   └──	LiquidCrystal_RW1073.h		# Modded LiquidCrystal.h lib for RW1073 controller		
├── src 							# Source files
|   ├── main.cpp					# main file
|   ├── uart_com.cpp				# uart functions
|	└──	LiquidCrystal_RW1073.cpp	# Modded LiquidCrystal.cpp lib for RW1073 controller		
├── README.txt                   
└── platformio.ini					# Project Configuration File


----------------------------------------------------------------------------------
