#!/bin/bash

if [[ $# != 2 ]]; then
  echo "Sposób uzytkowania: $0 <ścieżka/do/folderu/z/testami> <ścieżka/do/fodleru/z/projektem> " >&2
  exit 1
fi

tests=$(realpath "$1")
project=$(realpath "$2")

if ! [[ -d "$tests" ]]; then
  echo "Podany folder z testami nie istnieje"
  exit 1
fi

if ! [[ -d "$project" ]]; then
  echo "Podany folder z projektem nie istnieje"
  exit 1
fi

total=0
correct=0
leaked=0

function traverse_folder() {
  folder="$1"
  shopt -s nullglob
  for f in "$folder"/*.c; do
      run_test "$f"
  done
  shopt -s nullglob
  for f in "$folder"/*.cc; do
      run_test "$f"
  done

  shopt -s nullglob
  for d in "$folder"/*/; do
    echo "$d"
    traverse_folder "$(realpath "$d")"
  done
}

RED='\033[0;31m'
GREEN='\033[0;32m'
NOCOLOR='\033[0m'

function run_test() {
  input_file="$1"
  extension="${input_file##*.}"
  error_file=${input_file//.$extension/.err}

  ((total++))
  echo -e "\e[1mTest $f \e[0m"

  if [[ "$extension" == "c" ]]; then
    gcc -Wall -Wextra -O2 -std=c11 -c "$input_file" -o test_obj.o
  else
    g++ -Wall -Wextra -O2 -std=c++17 -c "$input_file" -o test_obj.o
  fi

  g++ test_obj.o encstrset.o -o compiled_test

  ./compiled_test 1>"$temp_out" 2>"$temp_err"
  rm compiled_test 2> /dev/null

    if [[ ! -s "$temp_out" ]]; then
        echo -ne "${GREEN}stdout ok (empty)${NOCOLOR}, "

        if cmp -s "$error_file" "$temp_err" ; then
            echo -ne "${GREEN}stderr ok${NOCOLOR}\n"
            ((correct++))
        else
            echo -ne "${RED}stderr nieprawidlowe${NOCOLOR}\n"
            sdiff -d "$error_file" "$temp_err"
            exit 1
        fi

    else
        echo -ne "${RED}stdout nieprawidlowe (niepuste)${NOCOLOR}\n"
        sdiff -d "$output_file" "$temp_out"
        exit 1
    fi
  }

temp_out=$(mktemp)
temp_err=$(mktemp)
trap 'rm -f "$temp_out"' INT TERM HUP EXIT
trap 'rm -f "$temp_err"' INT TERM HUP EXIT

cd $2
echo "Compilation of encstrset.cc..."
g++ -Wall -Wextra -O2 -std=c++17 -c encstrset.cc -o encstrset.o

traverse_folder "$tests"

rm test_obj.o encstrset.o 2> /dev/null

echo "total: ${total}, correct ${correct}"
