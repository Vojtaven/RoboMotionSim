#ifndef SERIAL_INPUT_HPP
#define SERIAL_INPUT_HPP

#include "AppConfig.hpp"
#include "RobotState.hpp"
#include <asio.hpp>
#include <optional>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

class SerialInput {
public:
	SerialInput(const SerialMapping& serialMapping);
	~SerialInput();
	std::optional<std::string> update(RobotState& state);
	void updateAfterSettingsChange();

private:
	void startReading();
	void stop();
	void readLoop();
	bool readExact(uint8_t* buffer, size_t size);

	const SerialMapping& _serialMapping;
	asio::io_context _ioContext;
	std::unique_ptr<asio::serial_port> _serialPort;
	std::thread _ioThread;
	std::atomic<bool> _running{ false };

	std::mutex _dataMutex;
	std::vector<float> _wheelSpeeds;
	bool _hasNewData = false;
	std::string _errorMessage;
};

#endif // !SERIAL_INPUT_HPP