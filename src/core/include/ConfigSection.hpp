#ifndef CONFIG_SECTION_HPP
#define CONFIG_SECTION_HPP

#include <vector>
#include <functional>
#include <cstdint>
#include <algorithm>

template<typename T>
class ConfigSection;

// RAII guard that unsubscribes on destruction
template<typename T>
class ScopedSubscription {
public:
	ScopedSubscription() = default;
	ScopedSubscription(ConfigSection<T>& section, uint32_t id)
		: _section(&section), _id(id) {}

	~ScopedSubscription() { reset(); }

	ScopedSubscription(const ScopedSubscription&) = delete;
	ScopedSubscription& operator=(const ScopedSubscription&) = delete;

	ScopedSubscription(ScopedSubscription&& other) noexcept
		: _section(other._section), _id(other._id) {
		other._section = nullptr;
	}
	ScopedSubscription& operator=(ScopedSubscription&& other) noexcept {
		reset();
		_section = other._section;
		_id = other._id;
		other._section = nullptr;
		return *this;
	}

	void reset() {
		if (_section) {
			_section->unsubscribe(_id);
			_section = nullptr;
		}
	}

private:
	ConfigSection<T>* _section = nullptr;
	uint32_t _id = 0;
};

template<typename T>
class ConfigSection {
public:
	using CallbackId = uint32_t;

	ConfigSection() = default;
	explicit ConfigSection(const T& data) : _data(data) {}
	explicit ConfigSection(T&& data) : _data(std::move(data)) {}

	// Copy only data, not callbacks. Notifies subscribers of this section.
	ConfigSection(const ConfigSection& other) : _data(other._data) {}
	ConfigSection& operator=(const ConfigSection& other) {
		_data = other._data;
		notify();
		return *this;
	}

	ConfigSection(ConfigSection&& other) noexcept : _data(std::move(other._data)) {}
	ConfigSection& operator=(ConfigSection&& other) noexcept {
		_data = std::move(other._data);
		notify();
		return *this;
	}

	const T& get() const { return _data; }

	void set(const T& newData) {
		_data = newData;
		notify();
	}

	void set(T&& newData) {
		_data = std::move(newData);
		notify();
	}

	// Subscribe with RAII — automatically unsubscribes when the returned guard is destroyed
	ScopedSubscription<T> scopedSubscribe(std::function<void(const T&)> callback) {
		CallbackId id = _nextId++;
		_callbacks.push_back({ id, std::move(callback) });
		return ScopedSubscription<T>(*this, id);
	}

	void unsubscribe(CallbackId id) {
		_callbacks.erase(
			std::remove_if(_callbacks.begin(), _callbacks.end(),
				[id](const auto& entry) { return entry.id == id; }),
			_callbacks.end());
	}

	// Mutable access without triggering notifications.
	// Use when you need to modify fields directly (e.g. UI editing, deserialization)
	// or access nested ConfigSections for subscription.
	T& getMutable() { return _data; }

private:
	void notify() {
		for (auto& entry : _callbacks) {
			entry.callback(_data);
		}
	}

	struct CallbackEntry {
		CallbackId id;
		std::function<void(const T&)> callback;
	};

	T _data{};
	std::vector<CallbackEntry> _callbacks;
	CallbackId _nextId = 1;
};

#endif // CONFIG_SECTION_HPP
