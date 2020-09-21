#pragma once

#include "psvm.h"
#include "maths.hpp"
#include "filestream.h"

#include <cmath>
#include <memory>
#include <unordered_map>
#include <random>
#include <iostream>



std::unordered_map < std::string, PS_Operator > PSBaseOperators
{
    // file
    // filename access file file
    {"file", [](PSVM& vm) {
        auto access = vm.popOperand();
        auto filename = vm.popOperand()->asString();

        auto f = std::make_shared<FileStream>(filename.c_str());
        auto tok = std::make_shared<PSToken>(f);

        vm.pushOperand(tok);
    }},

    // bytesavailable
    // file bytesavailable int
    {"bytesavailable", [](PSVM& vm) {

        auto f = vm.popOperand()->asFile();
        auto n = f->remaining();

        vm.pushOperand(make_shared<PSToken>((int)n));
    }},

    // closefile
    // file closefile -
    {"closefile", [](PSVM& vm) {
        auto f = vm.popOperand()->asFile();
        f->close();

    }},

    // fileposition
    // file fileposition int
    {"fileposition", [](PSVM& vm) {
        auto f = vm.popOperand()->asFile();
        auto pos = f->tell();
        
        vm.pushOperand(make_shared<PSToken>((int)pos));
    }},

    // flushfile
    // file flushfile -
    {"flushfile", [](PSVM& vm) {

        auto f = vm.popOperand()->asFile();

        //f->flushToFile();
    }},
        
    // read
    // file read int true
    // file read false
    {"read", [](PSVM& vm) {
        auto f = vm.popOperand()->asFile();

        if (f->isEOF()) {
            vm.pushOperand(make_shared<PSToken>(false));
        } else {
            // read a single character from the file, and put it on 
            // the stack as an integer
            int val = f->readOctet();
            vm.pushOperand(make_shared<PSToken>(val));
            vm.pushOperand(make_shared<PSToken>(true));
        }
    }},

    // resetfile
    // file resetfile -
    {"resetfile", [](PSVM& vm) {

        auto f = vm.popOperand()->asFile();
        f->seek(0);
    }},


    // run
    // filename run -
    {"run", [](PSVM& vm) {
        auto filename = vm.popOperand()->asString();
        vm.runFilename(filename);
    }},

    // setfileposition
    // file int setfileposition -
    {"setfileposition", [](PSVM& vm) {
        auto n = vm.popOperand()->asInt();
        auto f = vm.popOperand()->asFile();

        f->seek((size_t)n);
    }},

    // write
    // file int write -
    {"write", [](PSVM& vm) {
        auto c = vm.popOperand()->asInt();
        auto f = vm.popOperand()->asFile();

        f->writeOctet(c);

    }},


};


// readhexstring
// writehexstring
/*
// readstring
//   file string readstring substring bool
local function readstring(vm)
local str = vm.OperandStack:pop()
local src = vm.OperandStack : pop()
local n = str.capacity

str : reset()

local offset = 0
while (not src:isEOF()) and offset < n do
    local c, err = src : readOctet()
    str[offset] = c
    offset = offset + 1
    end

    vm.OperandStack : push(str)
    vm.OperandStack : push(not src:isEOF())

    return true
    end
    exports.readstring = readstring
*/
    // writestring
/*
    -- readline
    --   file string readline substring bool
    local function readline(vm)
    local CR = string.byte("\r")
    local LF = string.byte("\n")
    local sawnewline = false

    local str = vm.OperandStack:pop()
    local src = vm.OperandStack : pop()
    local n = str.capacity
    --print("length: ", n, str.capacity)
    --print("stream, remaining: ", src:remaining())
    --reset the string before usage
    str : reset()

    local offset = 0;
    while (not src:isEOF()) and offset < n do
        --print("readline:WHILE")
        local c = src:peekOctet()
        --print(c, string.char(c))
        if (c == CR) then
            if src : peekOctet(1) == LF then
                sawnewline = true
                src : skip(2)
                break
                end
                elseif c == LF then
                sawnewline = true
                src : skip(1)
                break
                end

                src : skip(1)
                str[offset] = c
                offset = offset + 1
                end

                --print("  OP:readline; str: ", str, sawnewline)
                vm.OperandStack:push(str)
                vm.OperandStack : push(sawnewline)

                return true
                end
                exports.readline = readline
*/

// token
// file token ps_token
//
/*
                local function token(vm)
                local f = vm.OperandStack:pop()
                local scnr = f.Scanner
                if not scnr then
                    -- if there's not already a scanner assigned to the
                    --file, then create oneand assign it
                    scnr = Scanner(vm, f)
                    f.Scanner = scnr
                    end

                    -- Just get the next available token
                    local foundOne = false
                    local foundTok = nil
                    for _, tok in scnr : tokens() do
                        foundTok = tok
                        print("OP:token: ", tok.kind, tok.value)
                        break
                        end

                        if foundTok ~= nil then
                            vm.OperandStack:push(tok)
                            vm.OperandStack : push(true)
                        else
                            vm.OperandStack:push(false)
                            end

                            return true
                            end

*/

// flush
// status

/*
                            -- currentfile
                            -- - currentfile file
                            local function currentfile(vm)
                            if vm.CurrentFile then
                                vm.OperandStack:push(vm.CurrentFile)
                                --vm.OperandStack : push(true)
                            else
                                vm.OperandStack:push(ps_common.NULL)
                                --vm.OperandStack : push(false)
                                end

                                return true
                                end
                                exports.currentfile = currentfile

*/
