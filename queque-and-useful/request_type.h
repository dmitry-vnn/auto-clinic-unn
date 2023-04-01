#pragma once

//CLIENT TO SERVER REQUEST TYPE
enum class ClientRequestType
{

	AUTH, //And next login and password data
	GET_PATIENT,
	LOGOUT

};


//SERVER TO CLIENT RESPONSE TYPE
enum class ServerResponseType {

	NO_RESPONSE_ERR,

	GET_PATIENT_OK, //And next patient from JSON
	QUEUE_IS_EMPTY_ERR,

	NOT_AUTH_ERR,
	IS_ALREADY_AUTH_ERR,
	WRONG_AUTH_ERR,
	AUTH_OK //And next Personal data
};



