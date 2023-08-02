#ifndef SERIAL_CONNECTION_HPP_
#define SERIAL_CONNECTION_HPP_

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>

class SerialConnection {
public:
    SerialConnection();
    void establishSerialConnection();
    void mmStep(int mm);
    void setHome();
    void home();

private:
    boost::asio::io_service io;
    boost::asio::serial_port serialPort{io};
};

#endif  // SERIAL_CONNECTION_HPP_


