#include <Interrupted.h>
#include <Tasks.h>

#define MAX	6

Semaphore forks[MAX];
static byte ids = 1;
static byte leds[] = { LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8 };

void eating(byte id, byte weight) {
	Serial.print("Philosopher ");
	Serial.print(id);
	Serial.print(" eats: ");
	Serial.println(weight);

	digitalWrite(leds[id], HIGH);
	delay(500);
	digitalWrite(leds[id], LOW);

	Tasks::ready(Tasks::current());
	Tasks::reschedule();
}

class Philosopher: public Task<50> {
public:
	void setup() {
		_id = ids++;
	}

	void loop() {
		forks[_id - 1].wait();
		forks[_id].wait();

		eating(_id, _w++);

		forks[_id - 1].signal();
		forks[_id].signal();
	}

private:
	byte _id, _w;
};

Philosopher philosophers[MAX-1];

void setup() {
	Serial.begin(9600);
	Tasks::init();
	for (int i = 0; i < MAX; i++) {
		forks[i].signal();
		pinMode(leds[i], OUTPUT);
	}
	for (int i = 0; i < MAX-1; i++)
		Tasks::start(philosophers[i]);
}

void loop() {
	static byte weight;

	forks[0].wait();
	forks[MAX-1].wait();

	eating(0, weight++);

	forks[0].signal();
	forks[MAX-1].signal();
}
