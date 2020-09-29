#include <iostream>
#include <string>
#include <list>
#include <sstream>
#include <TimeMapper/TimeMapper.h>
#include <cmath>
#include <algorithm>

std::string formatNumber(double value)
{
    double intPart = 0;
    const double fractPart = std::fabs(std::modf(value, &intPart));
    std::stringstream ss;
    if (value < 0) ss << '-';
    ss << std::fabs(intPart);
    const double fractDigits = std::rint(1000000 * fractPart);
    if (fractDigits == 0)
        ss << ".0";
    else
    {
        std::stringstream fractSS;
        fractSS << fractDigits;
        std::string fractS(fractSS.str());
        fractS.insert(std::begin(fractS), 6 - fractS.size(), '0');
        const auto notZero(std::find_if(std::rbegin(fractS), std::rend(fractS), [](char c) { return c != '0'; }));
        fractS.erase(notZero.base(), std::end(fractS));
        ss << '.' << fractS;
    }
    return ss.str();
}

void process(const std::string &line, TimeMapper &tm)
{
    std::stringstream ss(line);
    auto checkInput = [&ss] { if (!ss) throw std::runtime_error("Invalid input"); };
    std::string command;
    ss >> command; checkInput();
    if (command == "marker")
    {
        double beat = 0, sample = 0;
        ss >> beat >> sample; checkInput();
        tm.addMarker(beat, sample);
    }
    else if (command == "end_tempo")
    {
        double tempo = 1;
        ss >> tempo; checkInput();
        tm.setEndTempo(tempo);
    }
    else if (command == "b2s")
    {
        double beat = 1;
        ss >> beat; checkInput();
        std::cout << formatNumber(tm.sample(beat)) << std::endl;
    }
    else if (command == "s2b")
    {
        double sample = 1;
        ss >> sample; checkInput();
        std::cout << formatNumber(tm.beat(sample)) << std::endl;
    }
}

int Main()
{
    std::list<std::string> input;
    for (std::string line; std::getline(std::cin, line); )
        input.emplace_back(std::move(line));

    TimeMapper tm;
    for (auto& line : input)
        process(line, tm);

    return 0;
}

int main()
{
    try { return Main(); }
    catch (const std::bad_alloc &)
    {
        std::printf("Mem alloc error");
        return -1;
    }
    catch (const TimeMapper::OutOfRange &)
    {
        std::cout << "Out of range" << std::endl;
        return -2;
    }
    catch (const std::exception &a)
    {
        std::cout << a.what() << std::endl;
    }

    return -3;
}
