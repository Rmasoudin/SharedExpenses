#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

const char COMMA_DELIMITER = ',';
const char SEMI_COLON_DELIMITER = ';';
const char COLON_DELIMITER = ':';
const int START_OF_LINE = 0;
const float EQUALLY_SPLIT = 0;
const string EQUALLY = "equally";
const string UNEQUALLY = "unequally";
const string ADJUSTMENT = "adjustment";
typedef struct expenses expenses;
typedef struct sort_output output;
typedef const char route;
typedef vector<float> money;
typedef vector<string> name;
typedef vector<expenses> cost;
struct expenses {
  name names;
  money amount;
};

struct sort_output {
  name payer;
  name deptor;
  money amount;
};

void discard_first_line(ifstream &file) {
  string usless_line;
  getline(file, usless_line);
}

name extract_names(route *file_path) {
  name identity;
  string current_name;
  ifstream file;
  file.open(file_path);
  file.seekg(START_OF_LINE);
  discard_first_line(file);
  while (!file.eof()) {
    getline(file, current_name);
    identity.push_back(current_name);
  }
  file.close();
  return identity;
}

name extract_type_expenses(route *file_path) {
  name types;
  ifstream file;
  file.open(file_path);
  file.seekg(START_OF_LINE);
  discard_first_line(file);
  string each_line;
  while (getline(file, each_line)) {
    istringstream stream_line(each_line);
    string type_name;
    getline(stream_line, type_name, COMMA_DELIMITER);
    types.push_back(type_name);
  }
  file.close();
  return types;
}

name extract_payers_lines(route *file_path) {
  name payer_lines;
  ifstream file;
  file.open(file_path);
  file.seekg(START_OF_LINE);
  discard_first_line(file);
  string each_line;
  while (getline(file, each_line)) {
    size_t first_comma_pos = each_line.find(COMMA_DELIMITER);
    size_t second_comma_pos =
        each_line.find(COMMA_DELIMITER, first_comma_pos + 1);

    if (first_comma_pos != string::npos && second_comma_pos != string::npos) {
      string name = each_line.substr(first_comma_pos + 1,
                                     second_comma_pos - first_comma_pos - 1);
      payer_lines.push_back(name);
    }
  }
  file.close();
  return payer_lines;
}

name extract_colon_and_semi_colon(name lines, int i) {
  name result;
  stringstream stream_string(lines[i]);
  string stored;
  while (getline(stream_string, stored, SEMI_COLON_DELIMITER)) {
    int pos = stored.find(COLON_DELIMITER);
    if (pos != std::string::npos) {
      result.push_back(stored.substr(START_OF_LINE, pos));
      result.push_back(stored.substr(pos + 1));
    } else {
      result.push_back(stored);
    }
  }
  return result;
}

void extract_payer_data(name payers_lines, cost &payers) {
  name clean_vector;
  for (int i = 0; i < payers_lines.size(); i++) {
    expenses exp;
    clean_vector = extract_colon_and_semi_colon(payers_lines, i);
    for (int t = 0; t < clean_vector.size(); t++) {
      try {
        float if_float = stof(clean_vector[t]);
        exp.amount.push_back(if_float);
      } catch (const invalid_argument &ia) {
        exp.names.push_back(clean_vector[t]);
      }
    }
    payers.push_back(exp);
  }
}

name extract_borrowers_lines(route *file_path) {
  name borrower_lines;
  ifstream file;
  file.open(file_path);
  file.seekg(START_OF_LINE);
  discard_first_line(file);
  string each_line;
  while (getline(file, each_line)) {
    size_t first_comma_pos = each_line.find(COMMA_DELIMITER);
    size_t second_comma_pos =
        each_line.find(COMMA_DELIMITER, first_comma_pos + 1);
    if (second_comma_pos != string::npos) {
      string everything_after_second_comma =
          each_line.substr(second_comma_pos + 1);
      if (everything_after_second_comma.size() >> 1) {
        borrower_lines.push_back(everything_after_second_comma);
      } else {
        borrower_lines.push_back("EQUALLY_SPLIT");
      }
    }
  }
  file.close();
  return borrower_lines;
}

void extract_borrower_data(name borrowers_lines, cost &borrowers) {
  name clean_vector;
  for (int i = 0; i < borrowers_lines.size(); i++) {
    expenses exp;
    clean_vector = extract_colon_and_semi_colon(borrowers_lines, i);
    for (int j = 0; j < clean_vector.size(); j++) {
      if (j % 2 != 0) {
        try {
          float if_float = stof(clean_vector[j]);
          exp.amount.push_back(if_float);
        } catch (const invalid_argument &ia) {
          exp.amount.push_back(EQUALLY_SPLIT);
          exp.names.push_back(clean_vector[j]);
        }
      } else {
        exp.names.push_back(clean_vector[j]);
      }
    }
    borrowers.push_back(exp);
  }
}

expenses make_optimization(name Identity) {
  expenses optimization;
  for (int i = 0; i < Identity.size(); i++) {
    optimization.names.push_back(Identity[i]);
    optimization.amount.push_back(0);
  }
  return optimization;
}

float calc_amount(cost payers, int which_line) {
  float amounts = 0;
  for (int j = 0; j < payers[which_line].amount.size(); j++) {
    amounts += payers[which_line].amount[j];
  }
  return amounts;
}

float calc_average(float average) {
  average = floor(average * 100) / 100;
  return average;
}

void equally_everyone(cost payers, cost borrowers, expenses &optimization,
                      int which_line) {
  float amounts = calc_amount(payers, which_line);
  float average = amounts / optimization.names.size();
  average = calc_average(average);
  for (int i = 0; i < optimization.names.size() - 1; i++) {
    optimization.amount[i] -= average;
  }
  optimization.amount[optimization.names.size() - 1] -=
      (amounts - (optimization.names.size() - 1) * average);
}

void equally_split(cost payers, cost borrowers, expenses &optimization,
                   int which_line) {
  float amounts = calc_amount(payers, which_line);
  float average = amounts / borrowers[which_line].names.size();
  average = calc_average(average);
  int find;
  for (int t = 0; t < optimization.names.size(); t++) {
    if (optimization.names[t] ==
        borrowers[which_line].names[borrowers[which_line].names.size() - 1]) {
      find = t;
    }
  }
  for (int i = 0; i < borrowers[which_line].names.size(); i++) {
    for (int j = 0; j < optimization.names.size(); j++) {
      if (borrowers[which_line].names[i] == optimization.names[j] &&
          i != borrowers[which_line].names.size() - 1) {
        optimization.amount[j] -= average;
      }
      if (i == borrowers[which_line].names.size() - 1 &&
          borrowers[which_line].names[i] == optimization.names[j]) {
        optimization.amount[find] -=
            (amounts - (borrowers[which_line].names.size() - 1) * average);
      }
    }
  }
}

void unequally(cost borrowers, expenses &optimization, int which_line) {
  for (int i = 0; i < borrowers[which_line].names.size(); i++) {
    for (int j = 0; j < optimization.names.size(); j++) {
      if (borrowers[which_line].names[i] == optimization.names[j]) {
        optimization.amount[j] -= borrowers[which_line].amount[i];
      }
    }
  }
}

float adjustment_special(cost payers, cost borrowers, expenses &optimization,
                         int which_line) {
  float amounts = calc_amount(payers, which_line);
  for (int i = 0; i < borrowers[which_line].names.size(); i++) {
    for (int j = 0; j < optimization.names.size(); j++) {
      if (borrowers[which_line].names[i] == optimization.names[j]) {
        optimization.amount[j] -= borrowers[which_line].amount[i];
        amounts -= borrowers[which_line].amount[i];
      }
    }
  }
  return amounts;
}

void adjustment(cost payers, cost borrowers, expenses &optimization,
                int which_line) {
  int b = borrowers[which_line].names.size() - 1;
  int find;
  for (int i = 0; i < optimization.names.size(); i++) {
    if (borrowers[which_line].names[b] == optimization.names[i]) {
      find = i;
    }
  }
  float amounts =
      adjustment_special(payers, borrowers, optimization, which_line);
  float average = amounts / optimization.names.size();
  average = calc_average(average);
  for (int j = 0; j < optimization.names.size(); j++) {
    if (j == find) {
      continue;
    }
    optimization.amount[j] -= average;
  }
  optimization.amount[find] -=
      (amounts - (optimization.names.size() - 1) * average);
}

void optimization_payer(cost payers, expenses &optimization, int which_line) {
  for (int i = 0; i < optimization.names.size(); i++) {
    for (int t = 0; t < payers[which_line].names.size(); t++) {
      if (optimization.names[i] == payers[which_line].names[t]) {
        optimization.amount[i] += payers[which_line].amount[t];
      }
    }
  }
}

expenses sort_amount(expenses optimization) {
  int n = optimization.amount.size();
  bool swapped;
  do {
    swapped = false;
    for (int i = 1; i < n; ++i) {
      if (optimization.amount[i - 1] < optimization.amount[i]) {
        swap(optimization.amount[i - 1], optimization.amount[i]);
        swap(optimization.names[i - 1], optimization.names[i]);
        swapped = true;
      }
    }
    --n;
  } while (swapped);
  return optimization;
}

expenses sort_name(expenses optimization) {
  int n = optimization.names.size();
  bool swapped;
  for (int i = 0; i < n - 1; ++i) {
    swapped = false;
    for (int j = 0; j < n - i - 1; ++j) {
      if ((optimization.amount[j] == optimization.amount[j + 1] &&
           optimization.names[j] > optimization.names[j + 1])) {
        swap(optimization.names[j], optimization.names[j + 1]);
        swap(optimization.amount[j], optimization.amount[j + 1]);
        swapped = true;
      }
    }
    if (!swapped) {
      break;
    }
  }
  return optimization;
}

void add_payers_amounts(cost payers, expenses &optimization) {
  for (int i = 0; i < payers.size(); i++) {
    optimization_payer(payers, optimization, i);
  }
}

void calc_everyone_expenses(cost payers, cost borrowers, expenses &optimization,
                            name types) {
  for (int i = 0; i < types.size(); i++) {
    if (types[i] == EQUALLY && borrowers[i].names[0] == "EQUALLY_SPLIT") {
      equally_everyone(payers, borrowers, optimization, i);
    } else if (types[i] == EQUALLY) {
      equally_split(payers, borrowers, optimization, i);
    } else if (types[i] == UNEQUALLY) {
      unequally(borrowers, optimization, i);
    } else if (types[i] == ADJUSTMENT) {
      adjustment(payers, borrowers, optimization, i);
    }
  }
  add_payers_amounts(payers, optimization);
}

void extract_data(cost &borrowers, cost &payers, route *file_path) {
  name payer_lines = extract_payers_lines(file_path);
  name borrower_lines = extract_borrowers_lines(file_path);
  extract_payer_data(payer_lines, payers);
  extract_borrower_data(borrower_lines, borrowers);
}

void divide_and_sort_optimization(expenses &optimization) {
  optimization = sort_amount(optimization);
  optimization = sort_amount(optimization);
}

float correct_computer_percision(float total) {
  if ((total > -0.01 && total < 0) || (total < 0.01 && total > 0)) {
    total = 0;
  }
  return total;
}

bool everything_zero(expenses optimization) {
  for (int i = 0; i < optimization.names.size(); i++) {
    if (optimization.amount[i] != 0) {
      return false;
    }
  }
  return true;
}
void calc_optimization(expenses optimization, output &sorted_output) {
  int i = 0;
  int j = optimization.names.size() - 1;
  while (!everything_zero(optimization)) {
    float total = optimization.amount[i] + optimization.amount[j];
    total = correct_computer_percision(total);
    total = round(100 * total) / 100;
    if (total > 0) {
      sorted_output.payer.push_back(optimization.names[i]);
      sorted_output.deptor.push_back(optimization.names[j]);
      sorted_output.amount.push_back(abs(optimization.amount[j]));
      optimization.amount[i] = total;
      optimization.amount[j] = 0;
      j--;
    } else if (total < 0) {
      sorted_output.payer.push_back(optimization.names[i]);
      sorted_output.deptor.push_back(optimization.names[j]);
      sorted_output.amount.push_back(abs(optimization.amount[i]));
      optimization.amount[j] = total;
      optimization.amount[i] = 0;
      i++;
    } else if (total == 0) {
      sorted_output.payer.push_back(optimization.names[i]);
      sorted_output.deptor.push_back(optimization.names[j]);
      sorted_output.amount.push_back(abs(optimization.amount[j]));
      optimization.amount[i] = 0;
      optimization.amount[j] = 0;
      i++;
      j--;
    }
  }
}

output sort_output_amounnt(output optimization) {
  int n = optimization.amount.size();
  bool swapped;
  do {
    swapped = false;
    for (int i = 1; i < n; ++i) {
      if (optimization.amount[i - 1] < optimization.amount[i]) {
        swap(optimization.amount[i - 1], optimization.amount[i]);
        swap(optimization.payer[i - 1], optimization.payer[i]);
        swap(optimization.deptor[i - 1], optimization.deptor[i]);
        swapped = true;
      }
    }
    --n;
  } while (swapped);
  return optimization;
}

output sort_name(output optimization) {
  int n = optimization.deptor.size();
  bool swapped;
  for (int i = 0; i < n - 1; ++i) {
    swapped = false;
    for (int j = 0; j < n - i - 1; ++j) {
      if ((optimization.amount[j] == optimization.amount[j + 1] &&
           optimization.deptor[j] > optimization.deptor[j + 1])) {
        swap(optimization.deptor[j], optimization.deptor[j + 1]);
        swap(optimization.amount[j], optimization.amount[j + 1]);
        swapped = true;
      } else if (optimization.amount[j] == optimization.amount[j + 1] &&
                 optimization.deptor[j] == optimization.deptor[j + 1]) {
        swap(optimization.payer[j], optimization.payer[j + 1]);
        swap(optimization.amount[j], optimization.amount[j + 1]);
        swapped = true;
      }
    }
    if (!swapped) {
      break;
    }
  }
  return optimization;
}

int main(int argc, route *argv[]) {
  name identity = extract_names(argv[1]);
  name types = extract_type_expenses(argv[2]);
  cost payers;
  cost borrowers;
  expenses optimization = make_optimization(identity);
  output sorted_output;
  extract_data(borrowers, payers, argv[2]);
  calc_everyone_expenses(payers, borrowers, optimization, types);
  divide_and_sort_optimization(optimization);
  calc_optimization(optimization, sorted_output);
  sorted_output = sort_output_amounnt(sorted_output);
  sorted_output = sort_name(sorted_output);
  for (int i = 0; i < sorted_output.payer.size(); i++) {
    cout << sorted_output.deptor[i] << " -> " << sorted_output.payer[i] << ": "
         << sorted_output.amount[i] << endl;
  }
}
