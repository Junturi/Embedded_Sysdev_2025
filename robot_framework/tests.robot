*** Settings ***
Library   String
Library   SerialLibrary

*** Variables ***
${com}      COM9
${baud}     115200
${board}    nRF
${seq}      000120\r\n
${exp}      80X
${seq2}     001067\r\n
${error}    -3X

*** Test Cases ***
Connect Serial
	Log To Console  Connecting to ${board}
	Add Port  ${com}  baudrate=${baud}  encoding=ascii
	Port Should Be Open  ${com}
	Reset Input Buffer
	Reset Output Buffer

Serial Parse Correct Test
	Write Data   ${seq}   encoding=ascii 
	Log To Console   Send sequence ${seq}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console   Received ${read}
	
	Should Be Equal As Strings   ${read}    ${exp}
	Log To Console   Tested ${read} is same as ${exp}

    Reset Input Buffer
	Reset Output Buffer

Serial Parse Incorrect Test
    Write Data   ${seq2}   encoding=ascii 
	Log To Console   Send sequence ${seq2}

	${read} =   Read Until   terminator=58   encoding=ascii 

	Log To Console   Received ${read}
	
	Should Be Equal As Strings   ${read}    ${error}
	Log To Console   Tested ${read} is same as ${error}

    Reset Input Buffer
	Reset Output Buffer
	
Disconnect Serial
    [TearDown]  Delete Port  ${com}
	Log To Console  Disconnecting ${board}