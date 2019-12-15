// exp 0f lab 6.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <picosha2.h>
#include <iostream>
#include <thread>
#include <array>
#include <sstream>
#include <iomanip>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
using namespace std;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;



const int N = 4; // Размерность входных данных для хеширования
bool flag = true; // Флаг работы потоков

void mythread(int s)
{
	// s - инициализация генератора псевдослучайных чисел
	srand(s);
	std::array<unsigned char, N> data_array; // Входные данные для хеширования
	std::thread::id id = this_thread::get_id(); // Получаем идентификатор потока
	while (flag) // Условно бесконечный цикл, выходим при выходе из приложения
	{
		ostringstream sout; // Суда пишем входные данные
		sout << setfill('0') << hex;
		for (int i = 0; i < N; i++) {
			data_array[i] = rand();
			sout << setw(2) << (int)data_array[i];
		}
		string hash = picosha2::hash256_hex_string(data_array);

		// Вывод лога Уровень логирования для trace
		BOOST_LOG_TRIVIAL(trace) << " A  message: id=" << id << " data: " << sout.str() << " hesh=" << hash;
		if (hash.compare(60, 4, "0000") == 0) // Нашли искомый хеш (последние 4 нуля)
		{
			// Вывод лога Уровень логирования для info
			BOOST_LOG_TRIVIAL(info) << "!!!!!!!!! A info message: " << id << " data: " << sout.str() << " hesh=" << hash;
			cout << "Yes!\n"; // Вывод на экран то, что подобрали хеш
			//system("pause");
		}
	}
}


int main(int argc, char* argv[])
{
	int m = thread::hardware_concurrency(); // Сколько потоков может выполняться одновременно на текущем железе
	std::cout << "M=" << m << "\n";
	if (argc > 1) // Пытаем прочитать число создаваемых потоков с комендной строки
	{
		sscanf_s(argv[1], "%d", &m); // Чтение из строки целого числа
	}
	// Логирование на консоль
	//boost::log::add_console_log(std::cout, keywords::format = "[%TimeStamp%]: %Message%");
	logging::add_file_log // расширенная настройка
	(
		keywords::file_name = "log_%N.log", // Шаблон для имени файла
		keywords::rotation_size = 10 * 1024 * 1024, //	Обеспечить ротацию лог-файлов по размеру
		keywords::time_based_rotation = sinks::file::rotation_at_time_point{ 0, 0, 0 }, // Ротация лог файла в 00:00:00 каждый суток
		keywords::format = "[%TimeStamp%]: %Message%" // Текстовый формат
	);
	logging::core::get()->set_filter
	(
		logging::trivial::severity <= logging::trivial::info // Уровень логирования не выше info
	);
	/*
	Ниже представлены разные уровни логирования
	BOOST_LOG_TRIVIAL(trace) << str << " A trace severity message";
	BOOST_LOG_TRIVIAL(debug) << str << " A debug severity message";
	BOOST_LOG_TRIVIAL(info) << str << " An informational severity message";
	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	BOOST_LOG_TRIVIAL(error) << "An error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";*/
	// Создаем потоки динамически
	cout << "For start thread, press Enter!\n";
	cout << "For exit, press Enter!\n";
	cin.get(); // Ждем нажатия Enter
	thread** pTh = new thread * [m];
	for (int i = 0; i < m; i++) pTh[i] = new thread(mythread, i + 1);
	cout << "Thread Start Ok!\n";

	cin.get(); // Ждем нажатия Enter
	flag = false; // завершение потокоd
	for (int i = 0; i < m; i++) pTh[i]->join();
	delete[] pTh;
	cout << "Thread is end!\n";
	system("pause");
	return 0;
}
// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
