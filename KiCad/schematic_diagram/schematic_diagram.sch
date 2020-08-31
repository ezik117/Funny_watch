EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "FUNNY WATCH"
Date "2020-08-31"
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ez_library:TFT_ILI9341 LCD1
U 1 1 5F4803C0
P 6150 1200
F 0 "LCD1" H 6978 321 50  0000 L CNN
F 1 "TFT_ILI9341" H 6978 230 50  0000 L CNN
F 2 "" H 6550 1250 50  0001 C CNN
F 3 "" H 6550 1250 50  0001 C CNN
	1    6150 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 1450 5800 1450
Wire Wire Line
	6000 1950 5800 1950
Wire Wire Line
	6000 1750 5700 1750
Wire Wire Line
	6000 2150 5700 2150
Wire Wire Line
	5700 2150 5700 1750
Wire Wire Line
	5600 2250 5600 1550
Wire Wire Line
	5600 2250 6000 2250
$Comp
L MCU_ST_STM32F0:STM32F030F4Px U1
U 1 1 5F481F4A
P 3100 2250
F 0 "U1" H 2750 1500 50  0000 C CNN
F 1 "STM32F030F4Px" H 3450 1500 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 2700 1550 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00088500.pdf" H 3100 2250 50  0001 C CNN
	1    3100 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 1750 4400 1750
Wire Wire Line
	3600 1850 4400 1850
Wire Wire Line
	3600 1950 4400 1950
Wire Wire Line
	3600 2050 4400 2050
Wire Wire Line
	3600 2150 4400 2150
Wire Wire Line
	3600 2250 4400 2250
Wire Wire Line
	3600 2450 4400 2450
Wire Wire Line
	3600 2550 4400 2550
Wire Wire Line
	3600 2650 4400 2650
Wire Wire Line
	3600 2750 4400 2750
Wire Wire Line
	3600 2850 4400 2850
Entry Wire Line
	4400 1750 4500 1650
Entry Wire Line
	4400 1850 4500 1750
Entry Wire Line
	4400 1950 4500 1850
Entry Wire Line
	4400 2050 4500 1950
Entry Wire Line
	4400 2150 4500 2050
Entry Wire Line
	4400 2250 4500 2150
Entry Wire Line
	4400 2350 4500 2250
Entry Wire Line
	4400 2450 4500 2350
Entry Wire Line
	4400 2550 4500 2450
Entry Wire Line
	4400 2650 4500 2550
Entry Wire Line
	4400 2750 4500 2650
Entry Wire Line
	4400 2850 4500 2750
Text Label 4550 900  0    50   ~ 0
a[0..14]
Text Label 5300 1350 0    50   ~ 0
a3
Text Label 4400 1750 2    50   ~ 0
a0
Text Label 4400 1850 2    50   ~ 0
a1
Text Label 4400 1950 2    50   ~ 0
a2
Text Label 4400 2050 2    50   ~ 0
a3
Text Label 4400 2150 2    50   ~ 0
a4
Text Label 4400 2250 2    50   ~ 0
a5
Text Label 4400 2350 2    50   ~ 0
a6
Text Label 4400 2450 2    50   ~ 0
a7
Text Label 4400 2550 2    50   ~ 0
a9
Text Label 4400 2650 2    50   ~ 0
a10
Text Label 4400 2750 2    50   ~ 0
a13
Text Label 4400 2850 2    50   ~ 0
a14
Text Label 5300 1450 0    50   ~ 0
a6
Connection ~ 5800 1450
Wire Wire Line
	5600 1550 6000 1550
Wire Wire Line
	5800 1950 5800 1450
Connection ~ 5600 1550
Text Label 5300 1550 0    50   ~ 0
a7
Text Label 5300 1650 0    50   ~ 0
a2
Connection ~ 5700 1750
Text Label 5300 1750 0    50   ~ 0
a5
Text Label 5300 2350 0    50   ~ 0
a9
Text Label 5300 2450 0    50   ~ 0
a10
Text Label 5300 2550 0    50   ~ 0
a4
Text Label 5300 2750 0    50   ~ 0
b0
Text Label 5300 2850 0    50   ~ 0
b1
Text Label 5350 900  0    50   ~ 0
b[0..1]
Wire Wire Line
	6000 2050 5800 2050
Wire Wire Line
	5800 2050 5800 2850
Connection ~ 5800 2850
Wire Wire Line
	5800 2850 6000 2850
Wire Wire Line
	1550 3200 2100 3200
Entry Wire Line
	4400 3200 4500 3100
Text Label 4400 3200 2    50   ~ 0
b0
Wire Wire Line
	3100 1550 3100 1450
Wire Wire Line
	3100 1450 3200 1450
Wire Wire Line
	3200 1450 3200 1550
Wire Wire Line
	3200 1450 4400 1450
Connection ~ 3200 1450
Entry Wire Line
	4400 1450 4500 1350
Text Label 4400 1450 2    50   ~ 0
b2
Wire Wire Line
	3600 2350 4400 2350
$Comp
L ez_library:w25q64fw U2
U 1 1 5F4AEA0C
P 6350 3400
F 0 "U2" H 6575 3415 50  0000 C CNN
F 1 "w25q64fw" H 6575 3324 50  0000 C CNN
F 2 "" H 6350 3400 50  0001 C CNN
F 3 "" H 6350 3400 50  0001 C CNN
	1    6350 3400
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 5F4B4F73
P 1850 2450
F 0 "Y1" H 1850 2718 50  0000 C CNN
F 1 "8MHz" H 1850 2627 50  0000 C CNN
F 2 "" H 1850 2450 50  0001 C CNN
F 3 "~" H 1850 2450 50  0001 C CNN
	1    1850 2450
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5F4B57EE
P 1550 2950
F 0 "C1" H 1665 2996 50  0000 L CNN
F 1 "22pF" H 1665 2905 50  0000 L CNN
F 2 "" H 1588 2800 50  0001 C CNN
F 3 "~" H 1550 2950 50  0001 C CNN
	1    1550 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5F4B5B76
P 2100 2950
F 0 "C2" H 2215 2996 50  0000 L CNN
F 1 "22pF" H 2215 2905 50  0000 L CNN
F 2 "" H 2138 2800 50  0001 C CNN
F 3 "~" H 2100 2950 50  0001 C CNN
	1    2100 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 2450 1550 2650
Connection ~ 1550 2650
Wire Wire Line
	1550 2800 1550 2650
Wire Wire Line
	2100 2800 2100 2450
Connection ~ 2100 2450
Wire Wire Line
	2100 2450 2500 2450
Wire Wire Line
	1550 3200 1550 3100
Wire Wire Line
	2100 3100 2100 3200
Connection ~ 2100 3200
Wire Wire Line
	3100 3200 3100 3050
Connection ~ 3100 3200
Wire Wire Line
	3100 3200 4400 3200
Wire Wire Line
	2600 2550 2500 2550
Wire Wire Line
	2500 2550 2500 2450
Wire Wire Line
	1550 2650 2600 2650
Wire Wire Line
	7550 4250 7200 4250
Wire Wire Line
	7200 3650 7550 3650
Wire Wire Line
	7200 3750 7550 3750
Entry Wire Line
	7550 4250 7650 4150
Entry Wire Line
	7550 3650 7650 3550
Entry Wire Line
	7550 3750 7650 3650
Wire Wire Line
	5950 3650 5300 3650
Wire Wire Line
	5950 3750 5300 3750
Wire Wire Line
	5950 3950 5300 3950
Wire Wire Line
	5950 4250 5750 4250
Wire Wire Line
	5300 2850 5800 2850
Wire Wire Line
	5300 2750 6000 2750
Wire Wire Line
	5300 2550 6000 2550
Wire Wire Line
	6000 2450 5300 2450
Wire Wire Line
	6000 2350 5300 2350
Wire Wire Line
	5300 1750 5700 1750
Wire Wire Line
	6000 1650 5300 1650
Wire Wire Line
	5600 1550 5300 1550
Wire Wire Line
	5300 1450 5800 1450
Wire Wire Line
	5300 1350 6000 1350
Entry Wire Line
	5300 4250 5200 4150
Connection ~ 5200 900 
Wire Bus Line
	5200 900  7650 900 
Entry Wire Line
	5300 2850 5200 2750
Entry Wire Line
	5200 2650 5300 2750
Entry Wire Line
	5200 2450 5300 2550
Entry Wire Line
	5200 2350 5300 2450
Entry Wire Line
	5200 2250 5300 2350
Entry Wire Line
	5200 1650 5300 1750
Entry Wire Line
	5200 1550 5300 1650
Entry Wire Line
	5200 1450 5300 1550
Entry Wire Line
	5300 1450 5200 1350
Wire Bus Line
	4500 900  5200 900 
Entry Wire Line
	5300 1350 5200 1250
Entry Wire Line
	5300 3650 5200 3550
Entry Wire Line
	5300 3750 5200 3650
Entry Wire Line
	5300 3950 5200 3850
Text Label 7550 4250 2    50   ~ 0
b0
Text Label 5300 4250 0    50   ~ 0
b1
Text Label 5300 3650 0    50   ~ 0
a1
Text Label 5300 3750 0    50   ~ 0
a5
Text Label 5300 3950 0    50   ~ 0
a0
Text Label 7550 3650 2    50   ~ 0
a6
Text Label 7550 3750 2    50   ~ 0
a7
Text Notes 3650 2450 0    50   Italic 0
SPI MOSI
Text Notes 3650 2350 0    50   Italic 0
SPI MISO
Text Notes 3650 2250 0    50   Italic 0
SPI CLK
Text Notes 3650 1950 0    50   Italic 0
T_SEL
Text Notes 3650 1850 0    50   Italic 0
MEM_SEL
Text Notes 3650 1750 0    50   Italic 0
MEM_HOLD
Text Notes 3650 2050 0    50   Italic 0
T_IRQ
Text Notes 3650 2150 0    50   Italic 0
LCD_SEL
Text Notes 3650 2550 0    50   Italic 0
LCD_CMD
Text Notes 3650 2650 0    50   Italic 0
LCD_RESET
Wire Wire Line
	2000 2450 2100 2450
Wire Wire Line
	1700 2450 1550 2450
Wire Wire Line
	2100 3200 3100 3200
$Comp
L power:+3.3V #PWR0101
U 1 1 5F5183D5
P 1100 4850
F 0 "#PWR0101" H 1100 4700 50  0001 C CNN
F 1 "+3.3V" H 1115 5023 50  0000 C CNN
F 2 "" H 1100 4850 50  0001 C CNN
F 3 "" H 1100 4850 50  0001 C CNN
	1    1100 4850
	1    0    0    -1  
$EndComp
$Comp
L Device:Buzzer BZ1
U 1 1 5F518C0D
P 3900 3850
F 0 "BZ1" H 4053 3879 50  0000 L CNN
F 1 "1kHz Buzz" H 4053 3788 50  0000 L CNN
F 2 "" V 3875 3950 50  0001 C CNN
F 3 "~" V 3875 3950 50  0001 C CNN
	1    3900 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:D D1
U 1 1 5F5196B3
P 3500 3850
F 0 "D1" V 3500 3650 50  0000 L CNN
F 1 "1N4148.133" V 3600 3350 50  0000 L CNN
F 2 "" H 3500 3850 50  0001 C CNN
F 3 "~" H 3500 3850 50  0001 C CNN
	1    3500 3850
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 5F519A22
P 3050 4350
F 0 "R1" V 2843 4350 50  0000 C CNN
F 1 "1k" V 2934 4350 50  0000 C CNN
F 2 "" V 2980 4350 50  0001 C CNN
F 3 "~" H 3050 4350 50  0001 C CNN
	1    3050 4350
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT:PN2222A Q1
U 1 1 5F51A00A
P 3400 4350
F 0 "Q1" H 3590 4396 50  0000 L CNN
F 1 "PN2222A" H 3590 4305 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 3600 4275 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/PN/PN2222A.pdf" H 3400 4350 50  0001 L CNN
	1    3400 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 4150 3500 4000
Wire Wire Line
	3500 4000 3800 4000
Wire Wire Line
	3800 4000 3800 3950
Connection ~ 3500 4000
Wire Wire Line
	3500 3700 3800 3700
Wire Wire Line
	3800 3700 3800 3750
$Comp
L power:GND #PWR0102
U 1 1 5F5271D6
P 3500 4650
F 0 "#PWR0102" H 3500 4400 50  0001 C CNN
F 1 "GND" H 3505 4477 50  0000 C CNN
F 2 "" H 3500 4650 50  0001 C CNN
F 3 "" H 3500 4650 50  0001 C CNN
	1    3500 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 4550 3500 4650
$Comp
L power:+3.3V #PWR0103
U 1 1 5F5294F5
P 3500 3600
F 0 "#PWR0103" H 3500 3450 50  0001 C CNN
F 1 "+3.3V" H 3515 3773 50  0000 C CNN
F 2 "" H 3500 3600 50  0001 C CNN
F 3 "" H 3500 3600 50  0001 C CNN
	1    3500 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 3600 3500 3700
Connection ~ 3500 3700
Wire Wire Line
	2600 2850 2500 2850
Wire Wire Line
	2500 2850 2500 4350
Text Notes 2500 3800 1    50   Italic 0
BEEPER
Wire Wire Line
	2500 4350 2900 4350
Entry Wire Line
	7650 1100 7750 1200
Entry Wire Line
	7650 1200 7750 1300
Wire Wire Line
	7750 1200 8350 1200
Wire Wire Line
	8350 1300 7750 1300
Text Label 7750 1200 0    50   ~ 0
a13
Text Label 7750 1300 0    50   ~ 0
a14
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5F544ACF
P 8550 1300
F 0 "J1" H 8630 1342 50  0000 L CNN
F 1 "PROGRAM" H 8630 1251 50  0000 L CNN
F 2 "" H 8550 1300 50  0001 C CNN
F 3 "~" H 8550 1300 50  0001 C CNN
	1    8550 1300
	1    0    0    -1  
$EndComp
Entry Wire Line
	7650 1300 7750 1400
Wire Wire Line
	7750 1400 8350 1400
Text Label 7750 1400 0    50   ~ 0
b0
$Comp
L Device:R R2
U 1 1 5F567D9E
P 1450 6350
F 0 "R2" H 1520 6396 50  0000 L CNN
F 1 "100k" H 1520 6305 50  0000 L CNN
F 2 "" V 1380 6350 50  0001 C CNN
F 3 "~" H 1450 6350 50  0001 C CNN
	1    1450 6350
	1    0    0    -1  
$EndComp
Connection ~ 3050 6650
Wire Wire Line
	3850 6650 3850 5850
Wire Wire Line
	3050 6650 3850 6650
Connection ~ 3050 5400
Wire Wire Line
	3050 5400 3850 5400
Wire Wire Line
	3850 5400 3850 5550
Connection ~ 2500 5400
Wire Wire Line
	3050 5400 3050 5800
Wire Wire Line
	2500 5400 3050 5400
Wire Wire Line
	2500 6650 3050 6650
Connection ~ 2500 6650
Wire Wire Line
	2500 6500 2500 6650
Wire Wire Line
	3050 6650 3050 6500
Wire Wire Line
	2100 6650 2500 6650
Wire Wire Line
	2100 6500 2100 6650
Wire Wire Line
	2500 6000 2500 5950
Connection ~ 2500 6000
Wire Wire Line
	2750 6000 2500 6000
Wire Wire Line
	2500 6200 2500 6000
Connection ~ 1450 5750
Wire Wire Line
	1450 5750 1450 6200
Connection ~ 1450 5400
Wire Wire Line
	1450 5750 1450 5400
Wire Wire Line
	1650 5750 1450 5750
Wire Wire Line
	1950 5400 2500 5400
Wire Wire Line
	2500 5400 2500 5550
Connection ~ 2100 5750
Wire Wire Line
	1950 5750 2100 5750
Wire Wire Line
	1650 5400 1450 5400
Wire Wire Line
	2100 5750 2100 6200
Wire Wire Line
	2200 5750 2100 5750
$Comp
L Device:CP C0
U 1 1 5F568D12
P 3850 5700
F 0 "C0" H 3968 5746 50  0000 L CNN
F 1 "100uF" H 3968 5655 50  0000 L CNN
F 2 "" H 3888 5550 50  0001 C CNN
F 3 "~" H 3850 5700 50  0001 C CNN
	1    3850 5700
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT1
U 1 1 5F568343
P 3050 6400
F 0 "BT1" H 3168 6496 50  0000 L CNN
F 1 "CR2035" H 3168 6405 50  0000 L CNN
F 2 "" V 3050 6460 50  0001 C CNN
F 3 "~" V 3050 6460 50  0001 C CNN
	1    3050 6400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5F56768F
P 2500 6350
F 0 "R5" H 2570 6396 50  0000 L CNN
F 1 "10k" H 2570 6305 50  0000 L CNN
F 2 "" V 2430 6350 50  0001 C CNN
F 3 "~" H 2500 6350 50  0001 C CNN
	1    2500 6350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5F566CD8
P 1800 5750
F 0 "R3" V 1593 5750 50  0000 C CNN
F 1 "1k" V 1684 5750 50  0000 C CNN
F 2 "" V 1730 5750 50  0001 C CNN
F 3 "~" H 1800 5750 50  0001 C CNN
	1    1800 5750
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5F5665EF
P 2100 6350
F 0 "R4" H 2170 6396 50  0000 L CNN
F 1 "100k" H 2170 6305 50  0000 L CNN
F 2 "" V 2030 6350 50  0001 C CNN
F 3 "~" H 2100 6350 50  0001 C CNN
	1    2100 6350
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:PN2222A Q2
U 1 1 5F565605
P 2400 5750
F 0 "Q2" H 2590 5796 50  0000 L CNN
F 1 "PN2222A" H 2590 5705 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 2600 5675 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/PN/PN2222A.pdf" H 2400 5750 50  0001 L CNN
	1    2400 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 6500 1450 6700
Entry Wire Line
	4400 7050 4500 6950
Wire Wire Line
	1450 7050 4400 7050
Text Label 4400 7050 2    50   ~ 0
a10
$Comp
L power:GND #PWR0104
U 1 1 5F5CF551
P 1100 6800
F 0 "#PWR0104" H 1100 6550 50  0001 C CNN
F 1 "GND" H 1105 6627 50  0000 C CNN
F 2 "" H 1100 6800 50  0001 C CNN
F 3 "" H 1100 6800 50  0001 C CNN
	1    1100 6800
	1    0    0    -1  
$EndComp
Connection ~ 3850 6650
Entry Wire Line
	4500 5300 4400 5400
Wire Wire Line
	4400 5400 3850 5400
Connection ~ 3850 5400
Text Label 4400 5400 2    50   ~ 0
b1
Entry Wire Line
	4400 6650 4500 6550
Wire Wire Line
	3850 6650 4400 6650
Text Label 4400 6650 2    50   ~ 0
b0
Wire Wire Line
	2100 6650 1100 6650
Wire Wire Line
	1100 6650 1100 6800
Connection ~ 2100 6650
Wire Wire Line
	1100 5400 1450 5400
Entry Wire Line
	4500 4950 4400 5050
Text Label 4400 5050 2    50   ~ 0
b2
Wire Wire Line
	1100 4850 1100 5050
Wire Wire Line
	1100 5050 4400 5050
Connection ~ 1100 5050
Wire Wire Line
	1100 5050 1100 5400
Text Notes 3650 5050 2    50   Italic 0
POWER SUPPLY (LCD, MEM)
Text Notes 3800 5400 2    50   Italic 0
BACKUPED_POWER_SUPPLY
$Comp
L Device:R R0
U 1 1 5F5F858D
P 2100 1950
F 0 "R0" V 1893 1950 50  0000 C CNN
F 1 "10k" V 1984 1950 50  0000 C CNN
F 2 "" V 2030 1950 50  0001 C CNN
F 3 "~" H 2100 1950 50  0001 C CNN
	1    2100 1950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5F5FCE41
P 1600 1950
F 0 "#PWR0105" H 1600 1700 50  0001 C CNN
F 1 "GND" H 1605 1777 50  0000 C CNN
F 2 "" H 1600 1950 50  0001 C CNN
F 3 "" H 1600 1950 50  0001 C CNN
	1    1600 1950
	0    1    1    0   
$EndComp
Wire Wire Line
	2600 1950 2250 1950
Wire Wire Line
	1950 1950 1600 1950
Text Notes 3650 2750 0    50   Italic 0
PRG
Text Notes 3650 2850 0    50   Italic 0
PRG
Text Notes 2150 7050 0    50   Italic 0
POWER FAILURE DETECTION
Wire Wire Line
	5950 4050 5750 4050
Wire Wire Line
	5750 4050 5750 4250
Connection ~ 5750 4250
Wire Wire Line
	5750 4250 5300 4250
NoConn ~ 2600 1750
$Comp
L Transistor_FET:AO3401A Q3
U 1 1 5F4CF8F5
P 2950 6000
F 0 "Q3" H 3155 6046 50  0000 L CNN
F 1 "AO3401A" H 3155 5955 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3150 5925 50  0001 L CIN
F 3 "http://www.aosmd.com/pdfs/datasheet/AO3401A.pdf" H 2950 6000 50  0001 L CNN
	1    2950 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:D D3
U 1 1 5F4D28EC
P 1450 6850
F 0 "D3" V 1496 6770 50  0000 R CNN
F 1 "1N4148.133" V 1405 6770 50  0000 R CNN
F 2 "" H 1450 6850 50  0001 C CNN
F 3 "~" H 1450 6850 50  0001 C CNN
	1    1450 6850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1450 7000 1450 7050
$Comp
L Device:D_Schottky D2
U 1 1 5F4E0C7D
P 1800 5400
F 0 "D2" H 1800 5183 50  0000 C CNN
F 1 "SS12" H 1800 5274 50  0000 C CNN
F 2 "" H 1800 5400 50  0001 C CNN
F 3 "~" H 1800 5400 50  0001 C CNN
	1    1800 5400
	-1   0    0    1   
$EndComp
Text Notes 6200 6000 0    50   ~ 0
Note: PA10 is used for both: LCD reset and power failure detection. During the start the pin behavour is changed.
Wire Bus Line
	7650 900  7650 4150
Wire Bus Line
	5200 900  5200 4150
Wire Bus Line
	4500 900  4500 6950
$EndSCHEMATC
