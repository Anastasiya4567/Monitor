#include <iostream>
#include <deque>
#include <chrono>
#include <thread>

#include "monitor.h"

using namespace std;

deque<int> q;

//in seconds
const int PRODUCER_A_SLEEP = 7;
const int PRODUCER_B_SLEEP = 6;

const int CONSUMER_A_SLEEP = 8; 
const int CONSUMER_B_SLEEP = 10;

inline void waiting(int _time)
{
	std::this_thread::sleep_for(std::chrono::microseconds(_time*1000000));
}

inline void writing(deque<int> q)
{
	cout << "Store:  ";

	for (auto i = 0; i != q.size(); ++i)
	{
		cout << q.at(i) << " ";
	}
	cout << endl << endl;
}

class Buffer : private Monitor 
{
 public:
	Buffer(){}
	void producerEnter(int i);
	void consEvenEnter();
	void consOddEnter();	
	
 private:
	Condition full, empty, even, odd;
};

Buffer buffer;

void Buffer:: producerEnter(int i)
{
	enter();
	cout << "The producer tries to product something...\n";
	if(q.size() == 9) 
	{
		wait(full);
	}
	q.push_back(i);

	cout << "The producer products and puts into a store the number " << i << endl;

	writing (q);

	if (q.size() >= 5)
	{
		if (q.front()%2==0)
		{
			signal(even);
		}
		else
		{
			signal(odd);
		}
	}
	leave();
}

void Buffer:: consEvenEnter()
{
	enter();
	cout << "The consumer A tries to take something..." << endl << endl;
	if (q.size()<=4 || q.front()%2==1)
	{
		wait(even);
	}

	int a=q.at(0);
	q.pop_front();

	cout << "The consumer A takes from a store the even number " << a << endl;
		
	writing(q);

	signal(full);

	if (q.front()%2==1 && q.size()>4)
	{
		signal(odd);
	}
	leave();	
}

void Buffer:: consOddEnter()
{
	enter();
	cout << "The consumer B tries to take something..." << endl << endl;
	if (q.size()<=4 || q.front()%2==0)
	{
		wait(odd);
	}
	
	int a=q.at(0);
	q.pop_front();

	cout << "The consumer B takes from a store the even number " << a << endl;
	
	writing(q);

	signal(full);

	if (q.front()%2==0 && q.size()>4)
	{
		signal(even);
	}
	leave();
}	


void producerA()
{
	while (true)
	{
		int num=rand()%100+1;
	  	waiting(PRODUCER_A_SLEEP);
	  	buffer.producerEnter(num);
	}
}

void producerB()
{
	while (true)
	{
		int num=rand()%100+1;
	  	waiting(PRODUCER_B_SLEEP);
	  	buffer.producerEnter(num);
	}
}

void consumerA()
{
	while (true)
	{
		waiting(CONSUMER_A_SLEEP);
	  	buffer.consEvenEnter();
	}
}

void consumerB()
{
	while (true)
	{
	  	waiting(CONSUMER_B_SLEEP);
	  	buffer.consOddEnter();
	}
}


int main()
{
	thread thread1(producerA); //launch the threed of the  producer A
	thread thread2(producerB); //launch the threed of the  producer B
	thread thread3(consumerA); //launch the threed of the  consumer A 
	thread thread4(consumerB); //launch the threed of the  consumer B 

	usleep (120000000); //in microseconds (12 seconds)	

	thread1.detach();
	thread2.detach();
	thread3.detach();
	thread4.detach();

	return 0;
}