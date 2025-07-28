#pragma once

//일단 홀수 REQ 짝수 RES로? -> 테스트 단계 지나면 전면 수정해야할 것
enum class MessageType {
	DEFAULT = 0, //테스트용

	// 0~ 100 TEST용
	REQ_ECHO = 1,
	RES_ECHO = 2,

	REQ_DBECHO = 3,
	RES_DBECHO = 4,

	REQ_SELDB = 5,
	RES_SELDB = 6,

	// 101 ~ 200 COMMON
	RES_CLIENT_JOIN = 101,
};