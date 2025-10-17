# Week 6 I aim for 2 points. I have made the basic tests for time_parser function.

*** Settings ***

Library   String
Library   SerialLibrary

# Test Suite setup and teardown run at the beginning and end of the test suite
Suite Setup     Setup Suite
Suite Teardown  Teardown Suite

# Test setup and teardown run at beginning and end of each test
Test Setup       Common Setup
Test Teardown    Common Teardown

*** Variables ***

# Set up common variables
${com}      COM9
${baud}     115200
${board}    nRF

*** Test Cases ***

# Test with correctly formated test data
Serial Parse Correct Test
    # Set up the sequence to be sent and the expected results
    ${seq}=   Set Variable   000010\r
    ${exp}=   Set Variable   10X

	# Write sequence to UART
	Write Data   ${seq}   encoding=ascii 
	Log To Console   \nSend sequence ${seq}

    # Read reply from UART
	${read} =   Read Until   terminator=58   encoding=ascii 
	Log To Console  Received '${read}', Expected '${exp}'
	
	# Compare read and expected. Test passes if they are equal
	Should Be Equal As Strings   ${read}    ${exp}

Serial Parse Incorrect Seconds Test
    ${seq}=   Set Variable   000061\r
    ${exp}=   Set Variable   -3X
	Write Data   ${seq}   encoding=ascii 
	Log To Console   \nSend sequence ${seq}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console  Received '${read}', Expected '${exp}'
	
	Should Be Equal As Strings   ${read}    ${exp}

Serial Parse Incorrect Minutes Test
    ${seq}=   Set Variable   006100\r
    ${exp}=   Set Variable   -3X
	Write Data   ${seq}   encoding=ascii 
	Log To Console   \nSend sequence ${seq}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console  Received '${read}', Expected '${exp}'
	
	Should Be Equal As Strings   ${read}    ${exp}

Serial Parse Incorrect Hours Test
    ${seq}=   Set Variable   250000\r
    ${exp}=   Set Variable   -3X
	Write Data   ${seq}   encoding=ascii 
	Log To Console   \nSend sequence ${seq}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console  Received '${read}', Expected '${exp}'
	
	Should Be Equal As Strings   ${read}    ${exp}

Serial Parse Only Digits Test
    ${seq}=   Set Variable   00A010\r
    ${exp}=   Set Variable   -3X
	Write Data   ${seq}   encoding=ascii 
	Log To Console   \nSend sequence ${seq}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console  Received '${read}', Expected '${exp}'
	
	Should Be Equal As Strings   ${read}    ${exp}

Serial Parse Not Zero Test
    ${seq}=   Set Variable   000000\r
    ${exp}=   Set Variable   -3X
	Write Data   ${seq}   encoding=ascii 
	Log To Console   \nSend sequence ${seq}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console  Received '${read}', Expected '${exp}'
	
	Should Be Equal As Strings   ${read}    ${exp}

Serial Parse Length Test
    ${seq}=   Set Variable   00012\r
    ${exp}=   Set Variable   -2X
	Write Data   ${seq}   encoding=ascii 
	Log To Console   \nSend sequence ${seq}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console  Received '${read}', Expected '${exp}'
	
	Should Be Equal As Strings   ${read}    ${exp}

*** Keywords ***

Setup Console
    # Connect to the UART com port
    Log To Console  Connecting to ${board}
	Add Port  ${com}  baudrate=${baud}  encoding=ascii
	Port Should Be Open  ${com}
    Log To Console  Connected
	
Disconnect Serial
    # Disconnect from the UART com port
	Log To Console  Disconnecting ${board}
    Delete Port  ${com}
    Log To Console  Disconnected

# Run once when the test suite starts
Setup Suite
	Setup Console

# Run once when the test suite stops
Teardown Suite
    Disconnect Serial

# Run before every test
Common Setup
    # Reset input and output buffers to prevent extra characters
    Reset Input Buffer
	Reset Output Buffer

# Run after every test
Common Teardown
    Reset Input Buffer
	Reset Output Buffer