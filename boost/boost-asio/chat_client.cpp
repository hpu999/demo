//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//boost开发文档中实时聊天程序的客户端
//实现的大致思路是：在创建的客户端实例中初始化socket、连接服务器端，并且不断的进行着异步读操作（从服务器端读数据）
//在主线程中，从console中不断读取要被发送的消息，并且把这些消息post至io_service，然后进行异步写操作
//读写socket都是用异步操作
//这种方法不同于分别开一个读线程，一个写线程， 它的优势是线程不会一直等待读写数据，在并发数大的情况下通过异步读写能提高资源利用率


#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator)
    : io_service_(io_service),
      socket_(io_service) //使得成员函数能直接使用这些变量
  {
    boost::asio::async_connect(socket_, endpoint_iterator,
        boost::bind(&chat_client::handle_connect, this,
          boost::asio::placeholders::error)); //所有的操作都采用异步的方式
  }

  void write(const chat_message& msg)
  {
    io_service_.post(boost::bind(&chat_client::do_write, this, msg)); //将消息主动投递给io_service
  }

  void close()
  {
    io_service_.post(boost::bind(&chat_client::do_close, this)); //这个close函数是客户端要主动终止时调用  do_close函数是从服务器端
                                                                //读数据失败时调用
  }

private:

  void handle_connect(const boost::system::error_code& error)
  {
    if (!error)
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.data(), chat_message::header_length), //读取数据报头
          boost::bind(&chat_client::handle_read_header, this,
            boost::asio::placeholders::error));
    }
  }

  void handle_read_header(const boost::system::error_code& error)
  {
    if (!error && read_msg_.decode_header()) //分别处理数据报头和数据部分
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),//读取数据包数据部分
          boost::bind(&chat_client::handle_read_body, this,
            boost::asio::placeholders::error));
    }
    else
    {
      do_close();
    }
  }

  void handle_read_body(const boost::system::error_code& error)
  {
    if (!error)
    {
      std::cout.write(read_msg_.body(), read_msg_.body_length()); //输出消息
      std::cout << "\n";
      boost::asio::async_read(socket_, 
          boost::asio::buffer(read_msg_.data(), chat_message::header_length), //在这里读取下一个数据包头
          boost::bind(&chat_client::handle_read_header, this,  
            boost::asio::placeholders::error)); //完成一次读操作（处理完一个数据包）  进行下一次读操作
    }
    else
    {
      do_close();
    }
  }

  void do_write(chat_message msg)
  {
    bool write_in_progress = !write_msgs_.empty(); //空的话变量为false
    write_msgs_.push_back(msg); //把要写的数据push至写队列
    if (!write_in_progress)//队列初始为空 push一个msg后就有一个元素了
    {
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
          boost::bind(&chat_client::handle_write, this, 
            boost::asio::placeholders::error));
    }
  }

  void handle_write(const boost::system::error_code& error)//第一个消息单独处理，剩下的才更好操作
  {
    if (!error)
    {
      write_msgs_.pop_front();//刚才处理完一个数据 所以要pop一个
      if (!write_msgs_.empty())  
      {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
              write_msgs_.front().length()),
            boost::bind(&chat_client::handle_write, this,
              boost::asio::placeholders::error)); //循环处理剩余的消息
      }
    }
    else
    {
      do_close();
    }
  }

  void do_close()
  {
    socket_.close();
  }

private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], argv[2]); //输入ip（或域名）和端口号
    tcp::resolver::iterator iterator = resolver.resolve(query);

    chat_client c(io_service, iterator);

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    char line[chat_message::max_body_length + 1];
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {
      using namespace std; // For strlen and memcpy.
      chat_message msg;
      msg.body_length(strlen(line));
      memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
    }

    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
