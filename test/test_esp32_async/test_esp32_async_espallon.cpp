#include <Arduino.h>
#include <unity.h>

#include "../config.h"
/*
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <cstdlib>

void executeCommand(const boost::system::error_code & e) {
	int returnCode = system("node ../node_test/src_test/puppeteer_test.js");
	std::cout << "Código de retorno: " << returnCode << std::endl;
}
*/
int main() {
	/*
	boost::asio::io_context io;
	// boost::asio::steady_timer t(io, boost::asio::chrono::seconds(10));
	t.async_wait(&executeCommand);

	std::cout
		<< "El hilo principal continúa ejecutándose mientras el timer cuenta..."
		<< std::endl;
	io.run(); // Bloquea hasta que todos los trabajos asincrónicos se hayan
			  // completado.
	std::cout << "La tarea asincrónica ha completado." << std::endl;
*/
	return 0;
}

void setup() {
	UNITY_BEGIN();
	if (PRINTDEBUG)
		SERIALDEBUG.begin(115200);
	// RUN_TEST(test_esp_mac);
	UNITY_END();
}

void loop() {}