#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webServ.hpp"
using namespace std;

class Request2 {
    enum State {
        REQUEST_LINE,
        HEADERS,
        BODY
    };
    public:
        int client;
        char *buffer;
        std::string method;
        std::string path;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
        State state = REQUEST_LINE;
        bool lineComplete = true;
        bool headersComplete = false;
        std::string header;
        string file;
        bool bodyComplete = true;
        bool chunked = false;
        long content_length = LONG_MAX;
        int header_length = 0;
        ofstream bodyFile;


        Request2(int& client) {
            this->client = client;
            file = ".data" + std::to_string(client);
            //ofstream bodyFile(file, ios::out | ios::trunc);
            bodyFile.open(file, ios::out | ios::trunc | ios::binary);
            //bodyFile.close();
        }

        void handleBody(const char* buffer, size_t bufferLength, size_t pos) {
            if (chunked) {
                // Handling chunked transfer
                size_t i = pos;
                while (i < bufferLength) {
                    // Read chunk size
                    std::string chunkSizeStr;
                    for (; buffer[i] != '\r' && (i + 1) < bufferLength; ++i) {
                        chunkSizeStr += buffer[i];
                    }
                    i += 2; // Skip \r\n
                    int chunkSize = std::stoi(chunkSizeStr, nullptr, 16);
                    if (chunkSize == 0) break; // End of message

                    // Write chunk to file
                    std::ofstream bodyFile("body.dat", std::ios::binary | std::ios::app);
                    if (bodyFile) {
                        bodyFile.write(buffer + i, chunkSize);
                    }
                    i += chunkSize + 2; // Skip chunk data and trailing \r\n
            }
            } else if (content_length > 0) {
                // Non-chunked transfer, content_length must be set beforehand
                //std::ofstream bodyFile(file, std::ios::app);
                //if (content_length < 5000)
                //    getchar();
                if (bodyFile) {
                    size_t writeSize = std::min(static_cast<size_t>(content_length), bufferLength - pos);
                   // if (content_length < bufferLength - pos) {
                     //   writeSize = content_length;
                   // }
                    //cout << "\ncontent_length = " << content_length << " -pos = " << pos << " -bufferLength = " << bufferLength << " -writsize = " << writeSize << " -BufferStart = " << buffer[0] <<  endl;
                    // for (int i = 0; i < writeSize; i++) {
                    //     bodyFile << (buffer + pos)[i];
                    // }
                    bodyFile.write(buffer + pos, writeSize);
                    // if (content_length == 746) {
                    //     //cout << "buffer == " << buffer << endl;
                    //     for (int j = 0; j < writeSize; j++) {
                    //         cout << buffer[j];
                    //     }
                    //     for (int j = 0; j < writeSize; j++) {
                    //         bodyFile << buffer[j];
                    //     }
                    // }
                    bodyFile.flush();
                    content_length -= writeSize;
                    cout << "content_length = " << content_length << endl; 
                }
                //bodyFile.close();
            }
            if (content_length <= 0) {
                // End of message
                cout << "content_length =<< " << content_length << endl;
                state = REQUEST_LINE;
            }
        }

        void parse(const std::string& request) {
            std::istringstream requestStream(request);
            std::string line;
            header_length = 0;
            //cout << "request: " << request << endl;
            if (state == REQUEST_LINE) {
                // while (1);
                //cout << "request = " <<  buffer << endl;
                //cout.write(buffer, BUFFER_SIZE);
                std::getline(requestStream, line);
                std::istringstream lineStream(line);
                lineStream >> method >> path >> version;
                state = HEADERS;
                header_length = line.length() + 1;
                //cout << "Method line = " << line << endl; ///////////
            }
            while (state == HEADERS && std::getline(requestStream, line) ) {
               // cout << "Headers line = " << line << endl; ///////////
                //cout << "line = " << line << " \n>1st ch = " << (int)line[0] << endl;
                if (line[0] == '\r') {
                    headersComplete = true;
                    state = BODY;
                    cout << "headersComplete = " << headersComplete << endl;
                    break;
                }
                //cout << "2nd ch = " << (int)line[1] << endl;
                if (line.back() == '\r') {
                    //line.pop_back();
                    if (lineComplete) {
                        std::string::size_type pos = line.find(':');
                        header = line.substr(0, pos);
                        headers[header] = line.substr(pos + 2);
                        headers[header].pop_back();
                    }
                    else {
                        std::string::size_type pos = line.find('\r');
                        headers[header] += line.substr(0, pos);
                        if (headers[header].back() == '\r') {
                            headers[header].pop_back();
                        }
                    }
                    lineComplete = true;
                }
                else {
                    if (lineComplete) {
                        std::string::size_type pos = line.find(':');
                        header = line.substr(0, pos);
                        headers[header] = line.substr(pos + 2);
                    }
                    else {
                        headers[header] += line;
                    }
                    lineComplete = false; 
                }
                cout << "header => " << header << endl;
                if (header == "Transfer-Encoding" && headers[header] == "chunked") {
                    chunked = true;
                }
                if ( header == "Content-Length" ) { ////////////// it doesnt make it in here????
                    content_length = stol(headers[header]);
                    cout << "content_length = " << content_length << endl;
                }
                header_length += line.length() + 1;
                usleep(100);
            }
            int bodyStart = request.find("\r\n\r\n");
            if (bodyStart == -1) {
                bodyStart = 0;
            }
            else {
                bodyStart += 4;
            }

            if (state == BODY)
                handleBody(buffer, BUFFER_SIZE, bodyStart);
            // if (chunked && state == BODY) {
            //     buffer  = buffer + bodyStart;
            //     if (bodyComplete) {
            //         string chunk;
            //         getline(requestStream, chunk);
            //         int chunkSize = stoi(chunk, 0, 16);
            //         if (chunkSize == 0) {
            //             state = REQUEST_LINE;
            //             bodyComplete = false;
            //             return ;
            //         }
            //         bodyComplete = false;
            //     }
            //     ofstream bodyFile(file, ios::out | ios::app);
            //     while (getline(requestStream, body)) {
            //         if (body == "\r") {
            //             bodyComplete = true;
            //             break;
            //         }
            //         if (!requestStream.eof()) {
            //             // If we haven't reached the end of the stream, append '\n'
            //             body += '\n';
            //         }
            //         bodyFile << body;
            //     }
            //     bodyFile.close();
            // }
            // else if (!chunked && state == BODY) {
            //     buffer  = buffer + bodyStart;
            //     int size  = BUFFER_SIZE - bodyStart;
               
            //     ofstream bodyFile(file, ios::out | ios::app);

                
            //         const int chunk_size = std::min(content_length, BUFFER_SIZE);
                   
            //         cout << "size = " << size << endl;
                
            //         bodyFile.write(buffer, size);
            //         content_length -= size;

            //         if (content_length <= 0) {
            //             bodyComplete = false;
            //             state = REQUEST_LINE;
            //             //break;
            //         }
            //     bodyFile.close();
            // }
        }

            // Function to print all headers
        void printHeaders() const {
            std::cout << "Headers:" << std::endl;
            for (const auto& header : headers) {
                std::cout << header.first << ">> " << header.second << std::endl;
            }
        }

        ~Request2() {
            bodyFile.close();
        }

};

#endif