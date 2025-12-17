#include <iostream>
#include "SFML/Audio.hpp"
#include <thread>
#include <chrono>
#include <cctype>

constexpr int INT16LIMIT = 32767;
constexpr double PI = 3.1415926;
constexpr double MORSE_TIME_UNIT = 0.25;
constexpr double MORSE_FREQUENCY = 600;

/*
* Generates the samples for a soundwave of a specific frequency.
* @param frequency The frequency of the soundwave in hz
* @param duration The lenght of the sound in seconds | Default: 1s
* @param volume The volume of the sound in % | Default: 100%
* @param sampleRate The sampleRate of the sound in hz | Default: 44100 hz
*/
std::vector<std::int16_t> getFrequencySamples(double frequency,double duration = 1, unsigned volume = 100, const int sampleRate = 44100) {
	volume *= INT16LIMIT;
	volume /= 100;
	if (frequency < 0) {
		frequency *= -1;
	}
	double samplesPerCycle = double(sampleRate) / frequency;
	double stepSize = (2*PI) / samplesPerCycle;
	std::vector<std::int16_t> output;
	int samplesNum = sampleRate * duration;
	samplesNum = std::max(int(samplesPerCycle), samplesNum);
	samplesNum = std::round(double(samplesNum) / samplesPerCycle) * samplesPerCycle;
	for (int i = 0; i < samplesNum; i++) {
		double sinVal = std::cos(double(i) * stepSize);
		std::int16_t temp = sinVal * volume;
		output.push_back(temp);
	}
	output.push_back(0);
	return output;
}

struct sound {
	double duration;
	bool silent;
};

std::vector<sound> letterToMorseTimings(char c) {
	// Morse code representations for A-Z (lowercase index c-'a')
	static const char* codes[26] = {
		".-",    // a
		"-...",  // b
		"-.-.",  // c
		"-..",   // d
		".",     // e
		"..-.",  // f
		"--.",   // g
		"....",  // h
		"..",    // i
		".---",  // j
		"-.-",   // k
		".-..",  // l
		"--",    // m
		"-.",    // n
		"---",   // o
		".--.",  // p
		"--.-",  // q
		".-.",   // r
		"...",   // s
		"-",     // t
		"..-",   // u
		"...-",  // v
		".--",   // w
		"-..-",  // x
		"-.--",  // y
		"--.."   // z
	};

	std::vector<sound> timings;

	if (c == ' ') {
		// Word gap: 7 units of silence
		timings.push_back({ 7.0 * MORSE_TIME_UNIT,1 });
		return timings;
	}

	char lc = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	if (lc < 'a' || lc > 'z') {
		return timings; // unsupported char -> empty timings
	}

	const char* code = codes[lc - 'a'];
	for (size_t i = 0; code[i] != '\0'; ++i) {
		char sym = code[i];
		// Beep duration: dot=1, dash=3
		timings.push_back({ (sym == '.' ? 1.0 : 3.0) * MORSE_TIME_UNIT,0 });
		// Intra-character gap (silence) of 1 unit between symbols, except after last symbol
		if (code[i + 1] != '\0') {
			timings.push_back({ 1.0 * MORSE_TIME_UNIT,1 });
		}
	}
	// Inter-character gap (silence) of 3 units after the letter
	timings.push_back({ 3.0 * MORSE_TIME_UNIT,1 });

	return timings;
}

std::vector<std::int16_t> toMorseCode(std::string message) {
	std::vector<std::int16_t> output;
	for (int i = 0; i < message.length(); i++) {
		char c = message[i];
		std::vector<sound> timings = letterToMorseTimings(c);
		for (auto& it : timings) {
			std::vector<std::int16_t> samples = getFrequencySamples(MORSE_FREQUENCY, it.duration, (it.silent ? 0 : 100));
			for (auto& sample : samples) {
				output.push_back(sample);
			}
		}
	}
	return output;
}

int main() {
	std::cout << "Sound Test Application" << std::endl;

	/*
	//std::vector<std::int16_t> samples = getFrequencySamples(523.25,1);
	std::vector<std::int16_t> samples = toMorseCode("Hello World");
	std::vector<sf::SoundChannel> channels{ sf::SoundChannel::Mono };

	sf::SoundBuffer buffer;

	if (!buffer.loadFromSamples(samples.data(), samples.size(), 2, 44100, channels)) {
		std::cerr << "FAILED to load from samples\n";
	}

	sf::Sound sound(buffer);
	*/

	for (;;) {
		std::cout << "Enter your message: ";
		std::string userInput;
		std::getline(std::cin, userInput);
		if (userInput.empty()) {
			std::cout << "Please enter something!\n";
			continue;
		}
		std::vector<std::int16_t> samples = toMorseCode(userInput);
		std::vector<sf::SoundChannel> channels{ sf::SoundChannel::Mono };

		sf::SoundBuffer buffer;

		if (!buffer.loadFromSamples(samples.data(), samples.size(), 2, 44100, channels)) {
			//std::cerr << "FAILED to load from samples\n";
		}

		sf::Sound sound(buffer);
		sound.play();
		while (sound.getStatus() == sf::SoundSource::Status::Playing) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		
	}

	return 0;
}