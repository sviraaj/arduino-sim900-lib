#include "gsm.h"

Error Err;

gsm::gsm(void) {}
uint8_t gsm::at_set()
{
    ser.flush();
    gsm_write("AT\r");
    if (!check_status(AT))
    {
        // error_status(check_status(AT),AT);
        return 0;
    }

}  // check sim status
uint8_t gsm::sim_status()
{
    gsm_write("AT+CPIN?\r");
    if (!check_status(AT_CPIN))
    {
        // error_status(check_status(AT_CPIN),AT_CPIN);
        return 0;
    }
}
// check signal quality

uint8_t gsm::signal_quality()
{
    gsm_write("AT+CSQ\r");
    if (!check_status(AT_CSQ))
    {
        // error_status(check_status(AT_CSQ),AT_CSQ);
        return 0;
    }
}
// check card service provider
uint8_t gsm::card_pr()
{
    gsm_write("AT+COPS?\r");
    if (!check_status(AT_COPS))
    {
        // error_status(check_status(AT_COPS),AT_COPS);
        return 0;
    }
}
// check IMEI number
uint8_t gsm::imei_no()
{
    gsm_write("AT+CGSN\r");
    if (!check_status(AT_CGSN))
    {
        // error_status(check_status(AT_CGSN),AT_CGSN);
        return 0;
    }
}
// extended error code enable
uint8_t gsm::extended_error_code()
{
    gsm_write("AT+CMEE=1\r");
    if (!check_status(AT_CMEE))
    {
        // error_status(check_status(AT_CMEE),AT_CMEE);
        return 0;
    }

    gsm_write("AT&W\r");
    if (!check_status(AT_W))
    {
        // error_status(check_status(AT_W),AT_W);
        return 0;
    }

    return 1;
}

uint8_t gsm::gen_init()
{
    if (!at_set())
        return 0;
    if (!sim_status())
        return 0;
    if (!card_pr())
        return 0;
    if (!signal_quality())
        return 0;
    if (!imei_no())
        return 0;
    if (!extended_error_code())
        return 0;

    return 1;
}

uint8_t gsm::gprs_init()
{
    // check if gprs is attached
    gsm_write("AT+CGATT?\r");
    uint8_t status = check_status(AT_CGATT_q);
    if (!status)
    {
        // error_status(check_status(AT_CGATT_q),AT_CGATT_q);
        return 0;
    }

    // if not attached, attach gprs
    else if (status == DETACHED)
    {
        gsm_write("AT+CGATT=1\r");
        if (!check_status(AT_CGATT))
        {
            // error_status(check_status(AT_CGATT),AT_CGATT);
            return 0;
        }
    }
    else
        ;

    return 1;
}

uint8_t gsm::tcp_init()
{
    // check the status
    gsm_write("AT+CIPSTATUS\r");
    uint8_t status = check_status(AT_CIPSTATUS);
    if (!status)
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }

    // reset the previous tcp session if any
    while (status != IP_INITIAL)
    {
        gsm_write("AT+CIPSHUT\r");
        if (!check_status(AT_SHUT))
        {
            // error_status(check_status(AT_SHUT),AT_SHUT);
            return 0;
        }
        gsm_write("AT+CIPSTATUS\r");
        status = check_status(AT_CIPSTATUS);
        if (!status)
        {
            // error_status(check_status(AT_STATUS),AT_STATUS);
            return 0;
        }
    }

    // enable single ip connection
    gsm_write("AT+CIPMUX=0\r");
    if (!check_status(AT_CIPMUX))
    {
        // error_status(check_status(AT_CIPMUX),AT_CIPMUX);
        return 0;
    }

    // set task APN
    gsm_write("AT+CSTT=");
    gsm_write(APN);
    gsm_write(",\"\",\"\"\r");
    if (!check_status(AT_CSTT))
    {
        // error_status(check_status(AT_CSTT),AT_CSTT);
        return 0;
    }

    // bring up wireless connection with the GPRS
    // delay(1000);
    gsm_write("AT+CIICR\r");
    if (!check_status(AT_CIICR))
    {
        //	error_status(check_status(AT_CIICR),AT_CIICR);
        return 0;
    }

    // get the local IP address
    gsm_write("AT+CIFSR\r");
    if (!check_status(AT_CIFSR))
    {
        // error_status(check_status(AT_CIFSR),AT_CIFSR);
        return 0;
    }

    // show ip and port of received data
    gsm_write("AT+CIPSRIP=1\r");
    if (!check_status(AT_CIPSRIP))
    {
        // error_status(check_status(AT_CIPSRIP),AT_CIPSRIP);
        return 0;
    }

    return 1;
}

uint8_t gsm::http_init()
{
    gsm_write("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r");
    uint8_t status = check_http_status(AT_SAPBR);
    if (!status)
    {
        return 0;
    }

    gsm_write("AT+SAPBR=3,1,");
    gsm_write("\"APN\",");
    gsm_write(APN);
    gsm_write("\r");
    status = check_http_status(AT_SAPBR);
    if (!status)
    {
        return 0;
    }

    gsm_write("AT+SAPBR=2,1\r");
    status = check_http_status(AT_SAPBR_II);
    if (!status)
    {
        return 0;
    }

    while (status == 2)
    {
        gsm_write("AT+SAPBR=1,1\r");
        status = check_http_status(AT_SAPBR);
        if (!status)
        {
            return 0;
        }
        gsm_write("AT+SAPBR=2,1\r");
        status = check_http_status(AT_SAPBR_II);
        if (!status)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t gsm::at_http_init()
{
    // only if not initialised check later
    gsm_write("AT+HTTPINIT\r");
    uint8_t status = check_http_status(AT_HTTPINIT);
    if (!status)
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }
}

uint8_t gsm::gsm_init()
{
    Serial.flush();
    ser.flush();

    if (!gen_init())
        return 0;
    if (!gprs_init())
        return 0;
    if (!tcp_init())
        return 0;

    return 1;
}

uint8_t gsm::http_gsm_init()
{
    Serial.flush();
    ser.flush();

    if (!gen_init())
        return 0;

    if (!http_init())
        return 0;

    return 1;
}

uint8_t gsm::http_gsm_term()
{
    gsm_write("AT+HTTPTERM\r");

    if (!check_http_status(AT_HTTPTERM))
    {
        return 0;
    }
    return 1;
}

uint8_t gsm::connect_tcp(uint8_t mode, const String &addr, const String &port)
{
    // addr and port should have inherited "" i.e if connecting to google.com,
    // addr="\"www.google.com\""
    gsm_write("AT+CIPSTART=");
    if (mode == 0)
        gsm_write("\"TCP\",");
    if (mode == 1)
        gsm_write("\"UDP\",");
    ser.print(addr);
    gsm_write(",");
    ser.print(port);
    gsm_write("\r");
    if (!check_status(AT_CIPSTART))
    {
        // error_status(check_status(AT_CIPSTART),AT_CIPSTART);
        return 0;
    }

    return 1;
}

uint8_t gsm::send_data(String &send_str)
{
    gsm_write("AT+CIPSEND\r");
    if (!check_status(AT_CIPSEND))
    {
        // error_status(check_status(AT_CIPSEND),AT_CIPSEND);
        return 0;
    }
    ser.print(send_str);
    ser.write(0x1A);
    // CIPRECV not actually a command
    if (!check_status(AT_CIPRECV))
    {
        // error_status(check_status(AT_CIPRECV),AT_CIPRECV);
        return 0;
    }

    return 1;
}

uint8_t gsm::recv_data(String &recv_str)
{
    recv_str = "";
    uint8_t received = false;
    while (!received)
    {
        while (ser.available()) recv_str += (char)ser.read();
        ////////////TODO
    }
}

uint8_t gsm::http_set_para(String &url)
{
    gsm_write("AT+HTTPPARA=");
    gsm_write("\"CID\",");
    gsm_write("\"1\"");
    gsm_write("\r");

    if (!check_http_status(AT_HTTPPARA))
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }

    gsm_write("AT+HTTPPARA=");
    gsm_write("\"URL\",");
    ser.print(url);
    gsm_write("\r");

    if (!check_http_status(AT_HTTPPARA))
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }

    //////non-mandatory parameters required? like redirect ,break..... TODO

    return 1;
}

uint8_t gsm::http_send_transact(String &url, String &http_type,
                                sd_card *sd_read)
{
    String post_data = "";
    uint16_t data_length;
    uint16_t time_out = TIME_OUT;
    post_data += "card_id=" + sd_read->client_id + "&";
    post_data += "machine_id=" + sd_read->machine_id + "&";
    post_data += "product=" + sd_read->product_id + "&";
    post_data += "price=" + sd_read->price + "&";
    // post_data += "date_time=" + sd_read->date_time + "&";
    post_data += "payment_id=" + sd_read->payment_id + "&";
    post_data += "mode=" + sd_read->mode + "&";
    post_data += "name_cust=" + sd_read->name_cust + "&";
    post_data += "credit=" + sd_read->current_credit + "&";
    post_data += "coil=coil1&";
    return http_send_data(url, http_type, &post_data);
}

uint8_t gsm::http_send_data(String &url, String &http_type)
{
    return http_send_data(url, http_type, NULL);
}

uint8_t gsm::http_send_data(
    String &url, String &http_type,
    String *post_data)  // base method when doing inheritance
{
    uint16_t data_length;
    uint16_t time_out = TIME_OUT;
    data_length = post_data->length();

    String http_type_got = "";
    String status_code = "";
    String length = "";

    if (!http_set_para(url))
        return 0;

    if (post_data != NULL)
        if (!send_post_data(&data_length, &time_out, post_data))
            return 0;

    gsm_write("AT+HTTPACTION=");
    ser.print(http_type);
    gsm_write("\r");

    if (!check_http_status(AT_HTTPACTION))
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }
    else
    {
        if (action_parse(http_type_got, status_code, length))
        {
            DEBUG_PRINT_F("http_type_got:");
            DEBUG_PRINT(http_type_got);
            DEBUG_PRINT_F("staus_code:");
            DEBUG_PRINT(status_code);
            DEBUG_PRINT_F("length:");
            DEBUG_PRINT(length);
            Serial.println();
        }
        else
            return 0;
    }
}

uint8_t gsm::http_read()
{
    gsm_write("AT+HTTPREAD");
    gsm_write("\r");
    if (!check_http_status(AT_HTTPREAD))
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }

    return 1;
}

uint8_t gsm::send_post_data(uint16_t *data_length, uint16_t *time_out,
                            String *post_data)
{
    String content = "\"application/x-www-form-urlencoded\"";
    gsm_write("AT+HTTPPARA=");
    gsm_write("\"CONTENT\",");
    ser.print(content);
    gsm_write("\r");

    if (!check_http_status(AT_HTTPPARA))
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }

    gsm_write("AT+HTTPDATA=");
    ser.print(*data_length);
    ser.print(",");
    ser.print(*time_out);
    gsm_write("\r");

    if (!check_http_status(AT_HTTPDATA))
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }
    // fetch dataa
    ser.print(*post_data);

    if (!check_http_status(AT_HTTPDATA_C))
    {
        // error_status(check_status(AT_STATUS),AT_STATUS);
        return 0;
    }

    return 1;
}

uint8_t gsm::action_parse(String &http_type_got, String &status_code,
                          String &length)
{
    String received = "";
    uint8_t rec = 0;
    uint8_t temp = 0;
    while (!rec)
    {
        // Serial.println("rec");
        while (ser.available())
        {
            // Serial.println("avai");
            char c = (char)ser.read();
            received += c;
            if (temp == 1)
            {
                if (c != ',')
                    http_type_got += c;
                else
                {
                    temp++;
                    continue;
                }
            }
            if (temp == 2)
            {
                if (c != ',')
                    status_code += c;
                else
                {
                    temp++;
                    continue;
                }
            }
            if (temp == 3 && c == '\n')
            {
                rec = 1;
                break;
            }
            if (temp == 3)
            {
                length += c;
            }
            if (c == ':')
                temp++;
        }

    }  // while !rec
    DEBUG_PRINT(received);
    return 1;
}

uint8_t gsm::check_http_status(uint8_t command_http)
{
    String expected = "";
    String received = "";
    uint8_t rec = 0;
    String response = "";
    // Serial.println("check");
    switch (command_http)
    {
        case AT_SAPBR:
            // DEBUG_PRINT("AT+SAPBR=xxxxxxxx\r\r\n");

            expected = "AT+SAPBR=xxxxxx\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(rec);
                    if (rec == 2)
                        rec = 0;  // bug fix to be done in all cases ....
                                  // remember this.. if rec == 2 and ser
                                  // //not available, it exists unneccesarily..
                    if (rec == 1)
                    {
                        break;
                    }
                }

            }  // while !rec
            DEBUG_PRINT(received);
            // delay(1000);
            DEBUG_PRINT_F("response");
            DEBUG_PRINT_F("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_SAPBR, response, received);
                return 0;
            }
            break;

        case AT_SAPBR_II:

            expected = "AT+SAPBR\r\r\n+SAPBR: 1,1,xxxxxxxxxxxx\r\n";
            while (!rec)
            {
                // Serial.println("n");
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(received);
                    // Serial.println(rec);
                    if (rec == 1)
                    {
                        break;
                    }
                }

            }  // while !rec
            // DEBUG_PRINT(received);
            // DEBUG_PRINT("\r\n");
            if (response[6] != ':')
            {
                DEBUG_PRINT(F("prob"));
                error_status(AT_SAPBR_II, response, received);
                return 0;
            }
            // DEBUG_PRINT("1");
            // response = "";
            rec = 0;
            expected = "\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // DEBUG_PRINT(received);
                    // DEBUG_PRINT("rec:");Serial.println(rec);
                    if (rec == 1)
                    {
                        break;
                    }
                }

            }  // while !rec
            // DEBUG_PRINT("received");
            DEBUG_PRINT(received);
            // DEBUG_PRINT("response");
            // delay(1000);
            // DEBUG_PRINT_F("eee");
            // DEBUG_PRINT(response);
            DEBUG_PRINT_F("\r\n");
            if (response[10] != '1')
                return 2;
            return 1;
            // else { error_status(AT_SAPBR_II, response); return 0; }
            break;

        case AT_HTTPINIT:
            DEBUG_PRINT("AT+HTTPINIT\r\r\n");
            expected = "AT+HTTPINIT\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(rec);
                    if (rec == 1)
                        break;
                    // if (rec == 2) { error_status(AT, response); return 0; }
                    // //if response  is null string then the returned command
                    // itself is not right i.e. the sim900A is not communicating
                    // properly.
                }

            }  // while !rec
            // DEBUG_PRINT(received);
            DEBUG_PRINT(response);
            // delay(1000);
            DEBUG_PRINT_F("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_HTTPINIT, response, received);
                return 1;  //////////////////CHANGEEEEEEEEEEE
            }
            break;

        case AT_HTTPPARA:
            // DEBUG_PRINT("AT+HTTPPARA\r\r\n");
            expected = "AT+HTTPPARA=xxxxxxx\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(rec);
                    if (rec == 1)
                        break;
                }

            }  // while !rec
            DEBUG_PRINT(received);
            DEBUG_PRINT(response);
            DEBUG_PRINT_F("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_HTTPPARA, response, received);
                return 0;
            }
            break;

        case AT_HTTPDATA:
            // DEBUG_PRINT("AT+HTTPPARA\r\r\n");
            expected = "AT+HTTPDATA=xxxxxxx\r\r\nDOWNLOAD\r\n";
            while (!rec)
            {
                // Serial.println("rec");
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(rec);
                    if (rec == 1)
                        break;
                }

            }  // while !rec
            DEBUG_PRINT(received);
            DEBUG_PRINT(response);
            DEBUG_PRINT_F("\r\n");
            if (response = "DOWNLOAD")
                return 1;
            else
            {
                error_status(AT_HTTPDATA, response, received);
                return 0;
            }
            break;

        case AT_HTTPDATA_C:
            // DEBUG_PRINT("AT+HTTPPARA\r\r\n");
            expected = "\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(rec);
                    if (rec == 1)
                        break;
                }

            }  // while !rec
            DEBUG_PRINT(received);
            DEBUG_PRINT(response);
            DEBUG_PRINT_F("\r\n");
            if (response = "OK")
                return 1;
            else
            {
                DEBUG_PRINT_F("httpdatacontinue badd");
                error_status(AT_HTTPDATA, response, received);
                return 0;
            }
            response = "";
            rec = 0;
            expected = "\r\n";
            received += '\n';  // dummy \n to make it work with str_parse
            str_parse(expected, received, response);
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT_F("\r\n");
            break;

        case AT_HTTPACTION:
            // DEBUG_PRINT("AT+HTTPPARA\r\r\n");
            expected = "AT+HTTPAACTION=xxxxxxx\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(rec);
                    if (rec == 1)
                        break;
                }

            }  // while !rec
            DEBUG_PRINT(received);
            DEBUG_PRINT(response);
            DEBUG_PRINT_F("\r\n");
            if (response == "OK")
            {
                return 1;
            }
            else
            {
                error_status(AT_HTTPACTION, response, received);
                return 0;
            }
            break;

        case AT_HTTPTERM:
            // DEBUG_PRINT("AT+HTTPPARA\r\r\n");
            expected = "AT+HTTPTERM\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(rec);
                    if (rec == 1)
                        break;
                }

            }  // while !rec
            DEBUG_PRINT(received);
            DEBUG_PRINT(response);
            DEBUG_PRINT_F("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_HTTPTERM, response, received);
                return 0;
            }
            break;
    }
}

uint8_t gsm::check_status(uint8_t command)
{
    String expected = "";
    String received = "";
    uint8_t rec = 0;
    String response = "";
    Serial.println("check");
    switch (command)
    {
        case AT:
            DEBUG_PRINT_F("AT\r\r\n");
            expected = "AT\r\r\nOK\r\n";
            while (!rec)
            {
                // Serial.println("rec");
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println(rec);
                    if (rec == 1)
                        break;
                    // if (rec == 2) { error_status(AT, response , received);
                    // return 0; } //if response  is null string then the
                    // returned command itself is not right i.e. the sim900A is
                    // not communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT_F("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT, response, received);
                return 0;
            }
            break;

        case AT_CPIN:
            DEBUG_PRINT_F("AT+CPIN?\r\r\n");
            expected = "AT+CPIN?\r\r\n+CPIN: READY\r\n";
            while (!rec)
            {
                // Serial.println("n");
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println("rec:");
                    // Serial.println(rec);
                    if (rec == 1)
                    {
                        break;
                    }
                    if (rec == 2)
                    {
                        error_status(AT_CPIN, response, received);
                        return 0;
                    }
                    // if response  is null string then the returned command
                    // itself is not right i.e. the sim900A is not communicating
                    // properly.
                }

            }  // while !rec

            rec = 0;
            expected = "\r\nOK\r\n";  // wont reach \r\n ok \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // if(rec==2) {error_status(AT_W,response ,received);return
                    // 0;} //if response  is null string then the returned
                    // command itself is not right i.e. the sim900A is not
                    // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "+CPIN: READYOK")
                return 1;
            else
            {
                error_status(AT_CPIN, response, received);
                return 0;
            }
            break;

        case AT_CSQ:
            DEBUG_PRINT_F("AT+CSQ?\r\r\n");
            expected = "AT+CSQ?\r\r\n+CSQ:xx,0\r\n";
            while (!rec)
            {
                // Serial.println("csq");
                // Serial.print(ser.available());
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.print(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // Serial.println("rec:"); Serial.println(rec);
                    // if(rec==2) {error_status(AT_CSQ,response,
                    // received);return 0;} //if response  is null string then
                    // the returned command itself is not right i.e. the sim900A
                    // is not communicating properly.
                }

            }  // while !rec

            rec = 0;
            expected = "\r\nOK\r\n";  // wont reach \r\n ok \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // if(rec==2) {error_status(AT_W,response, received);return
                    // 0;} //if response  is null string then the returned
                    // command itself is not right i.e. the sim900A is not
                    // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");

            if (response != "ERROR")
            {
                DEBUG_PRINT(response);
                return 1;
            }
            else
            {
                error_status(AT_CSQ, response, received);
                return 0;
            }
            break;

        case AT_COPS:
            DEBUG_PRINT("AT+COPS?\r\r\n");
            expected = "AT+COPS?\r\r\nCOPS:xxxxxx\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // if(rec==2) {error_status(AT_CSQ,response);return 0;} //if
                    // response  is null string then the returned command itself
                    // is not right i.e. the sim900A is not communicating
                    // properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            return 1;
            // else {error_status(AT_CSQ,response);return 0;}
            break;

        case AT_CGSN:
            DEBUG_PRINT("AT+CGSN\r\r\n");
            expected = "AT+CGSN\r\r\nxxxxxx\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    if (rec == 2)
                    {
                        error_status(AT_CGSN, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            rec = 0;
            expected = "\r\nOK\r\n";  // wont reach \r\n ok \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // if(rec==2) {error_status(AT_W,response, received);return
                    // 0;} //if response  is null string then the returned
                    // command itself is not right i.e. the sim900A is not
                    // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            return 1;
            // else {error_status(AT_CSQ,response, received, );return 0;}
            break;

        case AT_CMEE:
            DEBUG_PRINT("AT+CMEE=1\r\r\n");
            expected = "AT+CMEE=1\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 2)
                    {
                        error_status(AT_CMEE, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_CMEE, response, received);
                return 0;
            }
            break;

        case AT_W:
            DEBUG_PRINT("AT&W\r\r\n");
            expected = "AT&W\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 2)
                    {
                        error_status(AT_W, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_W, response, received);
                return 0;
            }
            break;

        case AT_CGATT_q:
            DEBUG_PRINT("AT+CGATT?\r\r\n");
            expected =
                "AT+CGATT?\r\r\n+CGATT: x\r\n";  // wont reach \r\n ok \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // if(rec==2) {error_status(AT_W,response, received);return
                    // 0;} //if response  is null string then the returned
                    // command itself is not right i.e. the sim900A is not
                    // communicating properly.
                }

            }  // while !rec
            // else {error_status(AT_W,response, received);return 0;}
            DEBUG_PRINT("\r\n");
            rec = 0;
            expected = "\r\nOK\r\n";  // wont reach \r\n ok \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // if(rec==2) {error_status(AT_W,response ,received);return
                    // 0;} //if response  is null string then the returned
                    // command itself is not right i.e. the sim900A is not
                    // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            Serial.print("response[8]:");
            Serial.println(response[8]);
            if (response[8] == '0')
            {
                return DETACHED;
            }
            else
                return 1;
            // else {error_status(AT_W,response, received);return 0;}
            break;

        case AT_CGATT:
            DEBUG_PRINT("AT+CGATT=1\r\r\n");
            expected = "AT+CGATT=1\r\r\nOK\r\n";  // wont reach \r\n ok \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 2)
                    {
                        error_status(AT_CGATT, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_CGATT, response, received);
                return 0;
            }
            break;

        case AT_CIPSTATUS:
            DEBUG_PRINT("AT+CIPSTATUS\r\r\n");
            expected = "AT+CIPSTATUS\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // if(rec==2) {error_status(AT_CGATT,response ,
                    // received);return 0;} //if response  is null string then
                    // the returned command itself is not right i.e. the sim900A
                    // is not communicating properly.
                }

            }  // while !rec
            rec = 0;
            expected = "\r\nSTATE: IP xxxxxxx\r\n";  // wont reach \r\n ok \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // if(rec==2) {error_status(AT_W,response,received);return
                    // 0;} //if response  is null string then the returned
                    // command itself is not right i.e. the sim900A is not
                    // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            // delay(1000);
            if (response == "OKSTATE: IP INITIAL")
                return IP_INITIAL;
            else
                return 1;
            // else {error_status(AT_CGATT,response, received);return 0;}
            break;

        case AT_SHUT:
            DEBUG_PRINT("AT+CIPSHUT\r\r\n");
            expected = "AT+CIPSHUT\r\r\nSHUT OK\r\n";  // wont reach \r\n ok
                                                       // \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    if (rec == 2)
                    {
                        error_status(AT_SHUT, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            // delay(1000);
            if (response == "SHUT OK")
                return 1;
            else
            {
                error_status(AT_SHUT, response, received);
                return 0;
            }
            break;

        case AT_CIPMUX:
            DEBUG_PRINT("AT+CIPMUX=0\r\r\n");
            expected = "AT+CIPMUX=0\r\r\nOK\r\n";  // wont reach \r\n ok \r\n
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 2)
                    {
                        error_status(AT_CIPMUX, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_CIPMUX, response, received);
                return 0;
            }
            break;

        case AT_CSTT:
            DEBUG_PRINT("AT+CSTT=\"www\",\"\",\"\"\r\r\n");
            expected = "AT+CSTT=\"www\",\"\",\"\"\r\r\nOK\r\n";  // come back
                                                                 // later TODO
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 2)
                    {
                        error_status(AT_CSTT, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_CSTT, response, received);
                return 0;
            }
            break;

        case AT_CIICR:
            DEBUG_PRINT("AT+CIICR\r\r\n");
            expected = "AT+CIICR\r\r\nOK\r\n";
            while (!rec)
            {
                // Serial.println("ciicr");
                while (ser.available())
                {
                    // Serial.println("ciiavail");
                    received += (char)ser.read();
                    // Serial.println("receivedciicr:");
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // Serial.println("recciicr");
                    // Serial.println(rec);
                    if (rec == 1)
                        break;
                    if (rec == 2)
                    {
                        error_status(AT_CIICR, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_CIICR, response, received);
                return 0;
            }
            break;

        case AT_CIFSR:
            DEBUG_PRINT("AT+CIFSR\r\r\n");
            expected = "AT+CIFSR\r\r\nxxxxx\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    // if(rec==2) {error_status(AT_CIFSR,response,
                    // received);return 0;} //if response  is null string then
                    // the returned command itself is not right i.e. the sim900A
                    // is not communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            return 1;
            // else {error_status(AT_CIFSR,response, received);return 0;}
            break;

        case AT_CIPSRIP:
            DEBUG_PRINT("AT+CIPSRIP=1");
            expected = "AT+CIPSRIP=1\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 2)
                    {
                        error_status(AT_CIPSRIP, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "OK")
                return 1;
            else
            {
                error_status(AT_CIPSRIP, response, received);
                return 0;
            }
            break;
        ////////////make changes in this//////////////
        case AT_CIPSTART:
            DEBUG_PRINT("AT+CIPSTART=\"TCP\"xxxxxx");
            expected = "AT+CIPSTART=\"TCP\"xxxxxxxxxxxxx\r\r\nOK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                        break;
                    // if(rec==2) {error_status(AT_CIPSTART,response ,
                    // received);return 0;} //if response  is null string then
                    // the returned command itself is not right i.e. the sim900A
                    // is not communicating properly.
                }

            }  // while !rec
            rec = 0;
            expected = "\r\nCONNECT OK\r\n";
            DEBUG_PRINT(response);
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.println("received:");
                    // Serial.println(received);
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    // if(rec==2) {error_status(AT_W,response , received);return
                    // 0;} //if response  is null string then the returned
                    // command itself is not right i.e. the sim900A is not
                    // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "OKCONNECT OK")
                return 1;
            else
            {
                error_status(AT_CIPSTART, response, received);
                return 0;
            }
            break;

        //////////check AT+CIPSEND for errors///////////////////////
        case AT_CIPSEND:
            DEBUG_PRINT("AT+CIPSEND\r\r\n");
            expected = "AT+CIPSEND\r\r\n>";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    // Serial.print("received:");
                    // Serial.println(received);
                    if (received[received.length() - 1] == '>')
                    {
                        received += '\r';
                    }
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    if (received[received.length() - 1] == '>')
                    {
                        received += '\n';
                    }
                    if (rec == 2)
                    {
                        error_status(AT_CIPSEND, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == ">")
                return 1;
            else
            {
                error_status(AT_CIPSEND, response, received);
                return 0;
            }
            break;

        case AT_CIPRECV:
            while (!rec)
            {
                while (ser.available())
                {
                    char c = (char)ser.read();
                    if (c == 26)
                        break;
                    received += c;
                }

            }  // while !rec
            DEBUG_PRINT(received);
            DEBUG_PRINT("\r\n");

            expected = "\r\r\nSEND OK\r\n";
            while (!rec)
            {
                while (ser.available())
                {
                    received += (char)ser.read();
                    rec = str_parse(expected, received,
                                    response);  // gives zero i.e. false if
                                                // serial receive not ended.
                    if (rec == 1)
                    {
                        break;
                    }
                    if (rec == 2)
                    {
                        error_status(AT_CIPRECV, response, received);
                        return 0;
                    }  // if response  is null string then the returned command
                       // itself is not right i.e. the sim900A is not
                       // communicating properly.
                }

            }  // while !rec
            DEBUG_PRINT(response);
            DEBUG_PRINT("\r\n");
            if (response == "SEND OK")
                return 1;
            else
            {
                error_status(AT_CIPRECV, response, received);
                return 0;
            }
            break;

        default:
            break;

    }  // switch

}  // check_status

uint8_t gsm::str_parse(String &expected, String &received, String &response)
{
    static uint8_t n = 0;
    static uint8_t resp = 0;

    if (received[received.length() - 1] == '\n')
    {
        n++;
        // Serial.println("n:");
        // Serial.println(n);
        if (n == 1)
        {
            resp = 1;
            return 0;
        }
        if (n == 2)
        {
            n = 0;
            return 1;
        }
    }

    if (n == 0)
    {
        for (int i = 0; i < received.length(); i++)
        {
            if (expected[i] != received[i])
                ;  // return 2;    if return 2 take care in all cases..
        }
        return 0;
    }

    if (resp == 1)
    {
        if (received[received.length() - 1] == '\r')
            resp = 0;
        else
        {
            response += (char)received[received.length() - 1];
            // Serial.print("resp:");
            // Serial.print(response);
        }
    }
    return 0;
}

//////TODO functions

uint8_t gsm::close_tcp() { return 1; }
uint8_t gsm::error_status(uint8_t command, String &response, String &received)
{
    // http error_status
    // will finally change the post format to multipart/form-data
    uint8_t stat = Err.error_send(command, response, received);

    // for special functionalities for each commands
    switch (command)
    {
        case AT_SAPBR:
            break;
    }

    return stat;
}

uint8_t send_err()
{
    String file_name = ERROR;
    String read_line = "";
    int stat = Vend_sd.read_sd_nline(file_name, read_line);
    String post_data = "";
    uint8_t i = 0;
    // while((char)read_line[i]!="/"){
    // post_data+="error=" +
    // i++;
    //}

    return 1;
}

uint8_t gsm::detach_gprs() { return 1; }
