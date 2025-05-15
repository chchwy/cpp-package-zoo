#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <functional>
#include <memory>

// Timer example using Boost.Asio
void timer_example() {
    std::cout << "\n--- Asio Timer Example ---\n";
    
    boost::asio::io_context io;
    
    // Create a timer that expires after 2 seconds
    boost::asio::steady_timer timer(io, boost::asio::chrono::seconds(2));
    
    std::cout << "Timer started. Waiting for 2 seconds...\n";
    
    // Asynchronous wait
    timer.async_wait([](const boost::system::error_code& ec) {
        if (!ec) {
            std::cout << "Timer expired!\n";
        } else {
            std::cerr << "Error: " << ec.message() << "\n";
        }
    });
    
    // Run the io_context
    io.run();
}

// TCP echo server example using Boost.Asio
class TcpEchoServer {
public:
    TcpEchoServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
        std::cout << "TCP Echo Server started on port " << port << "\n";
        start_accept();
    }

private:
    void start_accept() {
        auto socket = std::make_shared<boost::asio::ip::tcp::socket>(acceptor_.get_executor());
        acceptor_.async_accept(*socket, [this, socket](const boost::system::error_code& ec) {
            if (!ec) {
                std::cout << "Client connected.\n";
                start_read(socket);
            }
            start_accept();
        });
    }

    void start_read(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
        auto buffer = std::make_shared<std::array<char, 1024>>();
        socket->async_read_some(
            boost::asio::buffer(buffer->data(), buffer->size()),
            [this, socket, buffer](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                if (!ec) {
                    std::cout << "Received: " << std::string(buffer->data(), bytes_transferred);
                    start_write(socket, buffer, bytes_transferred);
                }
            });
    }

    void start_write(
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        std::shared_ptr<std::array<char, 1024>> buffer,
        std::size_t bytes_transferred) {
        boost::asio::async_write(
            *socket, 
            boost::asio::buffer(buffer->data(), bytes_transferred),
            [this, socket, buffer](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
                if (!ec) {
                    start_read(socket);
                }
            });
    }

    boost::asio::ip::tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]) {
    namespace po = boost::program_options;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("timer", "run asio timer example")
        ("server", po::value<int>(), "run TCP echo server on specified port (default: 8080)");

    // Parse command line arguments
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    if (vm.count("timer")) {
        timer_example();
        return 0;
    }
    
    // Run the TCP echo server if requested
    if (vm.count("server")) {
        try {
            int port = vm["server"].empty() ? 8080 : vm["server"].as<int>();
            boost::asio::io_context io_context;
            TcpEchoServer server(io_context, port);
            std::cout << "Press Ctrl+C to exit...\n";
            io_context.run();
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
            return 1;
        }
        return 0;
    }

    // If no specific action is requested, show help
    std::cout << "Please specify --timer or --server to run an example.\n";
    std::cout << desc << "\n";
    
    return 0;
}