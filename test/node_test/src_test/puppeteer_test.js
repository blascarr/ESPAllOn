const puppeteer = require('puppeteer');
const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const config = require('./config');

const LOCAL_ESP_BOARD = config.LOCAL_ESP_BOARD;
const LOCAL_PC_PORT = config.LOCAL_PC_PORT;
const screenshotsFolder = config.screenshots_folder;

console.log(LOCAL_ESP_BOARD + ':' + LOCAL_PC_PORT);
const server = new WebSocket.Server({ port: LOCAL_PC_PORT });

getFormattedDate = () => {
	const now = new Date();
	const year = now.getFullYear();
	const month = String(now.getMonth() + 1).padStart(2, '0');
	const day = String(now.getDate()).padStart(2, '0');
	const hours = String(now.getHours()).padStart(2, '0');
	const minutes = String(now.getMinutes()).padStart(2, '0');
	const seconds = String(now.getSeconds()).padStart(2, '0');

	return `${year}-${month}-${day}-${hours}:${minutes}:${seconds}`;
};

runTest = async () => {
	const browser = await puppeteer.launch({ headless: false });
	const page = await browser.newPage();
	await page.goto(LOCAL_ESP_BOARD, { waitUntil: 'networkidle2' });

	// Wait until selector is available
	await page.waitForSelector('input', { visible: true });
	const selects = await page.$$('select');
	console.log(selects);
	if (selects.length > 0) {
		// Select First option
		const firstSelect = selects[1];
		await firstSelect.select('GPIO');
	}

	await page.waitForSelector('button', { visible: true });
	const saveButtons = await page.$$eval('button', (buttons) =>
		buttons.filter((button) => button.textContent.includes('Save'))
	);
	console.log(saveButtons);
	if (saveButtons.length > 0) {
		// Click on the first button
		await saveButtons[0].click();
	}

	// Optional: Capture Image from test execution
	const timestamp = getFormattedDate();
	const screenshotPath = `${screenshotsFolder}/result-${timestamp}.png`;
	if (!fs.existsSync(screenshotsFolder)) {
		fs.mkdirSync(screenshotsFolder);
	}

	await page.screenshot({ path: screenshotPath });

	// Close Browser
	await browser.close();
};

server.on('connection', (socket) => {
	console.log(`Client connected ${getFormattedDate()}`);

	socket.on('message', (message) => {
		console.log(`Message received from ESP : ${message}`);
		// Test is executed when signal from ESP is received
		if (message == 'startTest') {
			console.log('Execute Test');
			runTest();
		}
	});

	socket.on('disconnect', () => {
		console.log('Disconnected Client');
	});
});
