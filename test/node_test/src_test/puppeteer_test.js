const puppeteer = require('puppeteer');
const express = require('express');
const http = require('http');
const socketIo = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

runTest = async () => {
	const browser = await puppeteer.launch({ headless: false });
	const page = await browser.newPage();
	await page.goto('http://192.168.1.115');

	// Esperar a que el desplegable esté disponible y seleccionar una opción
	const selects = await page.$$('select');
	if (selects.length > 0) {
		// Seleccionar la primera opción del primer select
		const firstSelect = selects[0];
		await firstSelect.select('GPIO');
	}

	const saveButtons = await page.$$eval('button', (buttons) =>
		buttons.filter((button) => button.textContent.includes('Save'))
	);
	if (saveButtons.length > 0) {
		// Hacer clic en el primer botón
		await saveButtons[0].click();
	}
	// Opcional: Captura de pantalla después de hacer clic en el botón
	await page.screenshot({ path: 'screenshots/resultado.png' });

	// Cerrar el navegador
	await browser.close();
};

io.on('connection', (socket) => {
	console.log('Un cliente se ha conectado');

	// Escuchar una señal específica, por ejemplo 'startTest'
	socket.on('startTest', () => {
		console.log('Señal recibida para iniciar el test');
		// Aquí puedes colocar la lógica para ejecutar el test
		runTest(); // Asumimos que esta función ejecuta el test de Puppeteer
	});

	socket.on('disconnect', () => {
		console.log('Cliente desconectado');
	});
});

const port = 3000;
server.listen(port, () => {
	console.log(`Servidor escuchando en http://localhost:${port}`);
});
// runTest();
