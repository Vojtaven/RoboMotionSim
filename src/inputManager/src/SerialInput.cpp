#include "SerialInput.hpp"
#include <cstring>

SerialInput::SerialInput(const SerialMapping& serialMapping)
	: _serialMapping(serialMapping)
{
	startReading();
}

SerialInput::~SerialInput() {
	stop();
}

void SerialInput::startReading() {
	stop();

	try {
		_serialPort = std::make_unique<asio::serial_port>(_ioContext, _serialMapping.portName);
		_serialPort->set_option(asio::serial_port_base::baud_rate(_serialMapping.baudRate));
		_serialPort->set_option(asio::serial_port_base::character_size(8));
		_serialPort->set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
		_serialPort->set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
		_serialPort->set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
	}
	catch (const asio::system_error& e) {
		std::lock_guard<std::mutex> lock(_dataMutex);
		_errorMessage = std::string("Failed to open serial port: ") + e.what();
		_serialPort.reset();
		return;
	}

	_running = true;
	_ioThread = std::thread([this]() { readLoop(); });
}

void SerialInput::stop() {
	_running = false;
	if (_serialPort && _serialPort->is_open()) {
		asio::error_code ec;
		_serialPort->cancel(ec);
		_serialPort->close(ec);
	}
	if (_ioThread.joinable()) {
		_ioThread.join();
	}
	_serialPort.reset();
}

bool SerialInput::readExact(uint8_t* buffer, size_t size) {
	size_t totalRead = 0;
	while (totalRead < size && _running) {
		asio::error_code ec;
		size_t n = _serialPort->read_some(asio::buffer(buffer + totalRead, size - totalRead), ec);
		if (ec) {
			std::lock_guard<std::mutex> lock(_dataMutex);
			_errorMessage = std::string("Serial read error: ") + ec.message();
			return false;
		}
		totalRead += n;
	}
	return _running;
}

void SerialInput::readLoop() {
	while (_running) {
		// Read motor count (1 byte)
		uint8_t motorCount = 0;
		if (!readExact(&motorCount, 1))
			break;

		if (motorCount == 0 || motorCount > 64) {
			// Invalid motor count, skip
			continue;
		}

		// Read motorCount floats (4 bytes each)
		std::vector<float> speeds(motorCount);
		if (!readExact(reinterpret_cast<uint8_t*>(speeds.data()), motorCount * sizeof(float)))
			break;

		{
			std::lock_guard<std::mutex> lock(_dataMutex);
			_wheelSpeeds = std::move(speeds);
			_hasNewData = true;
			_errorMessage.clear();
		}
	}
}

std::optional<std::string> SerialInput::update(RobotState& state) {
	std::lock_guard<std::mutex> lock(_dataMutex);

	if (!_errorMessage.empty()) {
		return _errorMessage;
	}

	if (!_hasNewData) {
		return std::nullopt;
	}

	_hasNewData = false;
	state.fromWheelSpeeds = true;
	size_t count = std::min(_wheelSpeeds.size(), state.wheels.size());
	for (size_t i = 0; i < count; ++i) {
		state.wheels[i].speed = _wheelSpeeds[i];
	}

	return std::nullopt;
}

void SerialInput::updateAfterSettingsChange() {
	startReading();
}
