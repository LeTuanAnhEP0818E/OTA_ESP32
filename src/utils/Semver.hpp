/*
- A RAII Class Wrapper for semver's C-code
*/
#pragma once

#include "semver.h"

class Semver
{
public:
    Semver(const char *curr_ver, const char *comp_ver)
    {
        parse_current = semver_parse(curr_ver, &current_version);
        parse_compare = semver_parse(comp_ver, &compare_version);
        valid_input = (parse_current == 0 && parse_compare == 0);
        if (valid_input)
        {
            newer_version = (semver_compare(compare_version, current_version) > 0);
        }
    }
    ~Semver()
    {
        semver_free(&current_version);
        semver_free(&compare_version);
    }
    bool is_valid_input()
    {
        return valid_input;
    }
    bool is_newer_version()
    {
        return newer_version;
    }

private:
    semver_t current_version = {};
    semver_t compare_version = {};
    int parse_current;
    int parse_compare;
    bool valid_input{false};
    bool newer_version{false};
};