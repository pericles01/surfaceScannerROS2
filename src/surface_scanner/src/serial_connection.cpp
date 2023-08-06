#include "surface_scanner/serial_connection.hpp"

void SerialConnection::establishSerialConnection() {
    serialPort.open("/dev/ttyACM0");
    serialPort.set_option(boost::asio::serial_port_base::baud_rate(115200));
    serialPort.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    serialPort.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serialPort.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serialPort.set_option(boost::asio::serial_port_base::character_size(8));
    std::string in_bytes = "\r\n\r\n";
    boost::asio::write(serialPort, boost::asio::buffer(in_bytes));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //serialPort.flush();
}

void SerialConnection::mmStep(int mm) {
    std::string command = "G1 Y" + std::to_string(mm) + " F3000 \n";
    boost::asio::write(serialPort, boost::asio::buffer(command));
}

void SerialConnection::setHome() {
    boost::asio::write(serialPort, boost::asio::buffer("G91 \n"));
    boost::asio::write(serialPort, boost::asio::buffer("G28 \n"));
}

void SerialConnection::home() {
    boost::asio::write(serialPort, boost::asio::buffer("G28 \n"));
}
