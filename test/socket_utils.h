#include <WebSocketsClient.h>

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
	switch (type) {
	case WStype_DISCONNECTED:
		DUMPSLN("[WebSocket] Desconectado\n");
		break;
	case WStype_CONNECTED:
		DUMPSLN("[WebSocket] Conectado\n");
		break;
	case WStype_TEXT:
		Serial.printf("[WebSocket] Mensaje recibido: %s\n", payload);
		break;
	case WStype_ERROR:
	case WStype_FRAGMENT_TEXT_START:
	case WStype_FRAGMENT_BIN_START:
	case WStype_FRAGMENT:
	case WStype_FRAGMENT_FIN:
		break;
	}
}

void sendSocket() {
	if (webSocket.isConnected()) {
		DUMPSLN("SEND Start to WebSocket");
		webSocket.sendTXT("startTest");
	} else {
		DUMPSLN("WebSocket NOT CONNECTED");
	}
}

Ticker test_socketTicker(sendSocket, 5000, 1, MILLIS);

void initSocketProvider() {
	webSocket.begin(LOCAL_PC, LOCAL_PC_PORT);
	webSocket.onEvent(webSocketEvent);
	test_socketTicker.start();
}

void loopSocket() {
	webSocket.loop();
	test_socketTicker.update();
}