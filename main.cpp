#include <cmath>
#include <iostream>
#include <map>

using namespace std;

string vectorToKey(const vector<int>& nums) {
  string key;

  for (int num : nums) {
    key += to_string(num) + ",";
  }

  return key;
}

struct SolveResult {
  string formatString;
  vector<int> order;
};

SolveResult solve(vector<int> nums, map<string, string>& solveDict,
                  map<string, vector<int>>& orderDict,
                  bool allowPower = false) {
  sort(nums.begin(), nums.end());
  string key = vectorToKey(nums);

  if (solveDict.find(key) != solveDict.end()) {
    return {solveDict[key], orderDict[key]};
  } else if (nums.size() == 1) {
    if (nums[0] == 10) {
      return {"{}", vector<int>{0}};
    } else {
      return {"", vector<int>()};
    }
  } else {
    for (size_t i = 0; i < nums.size() - 1; i++) {
      for (size_t j = i + 1; j < nums.size(); j++) {
        for (int k = 0; k < 6; k++) {
          int newVal;
          string newFormat;
          bool skipOperation = false;

          switch (k) {
            case 0:
              newVal = nums[j] + nums[i];
              newFormat = "{}+{}";
              break;
            case 1:
              newVal = nums[j] - nums[i];
              newFormat = "{}-{}";
              break;
            case 2:
              newVal = nums[j] * nums[i];
              newFormat = "{}*{}";
              break;
            case 3:
              if (nums[i] == 0 || nums[j] % nums[i] != 0) {
                skipOperation = true;
                continue;
              }

              newVal = nums[j] / nums[i];
              newFormat = "{}/{}";
              break;
            case 4:
            case 5:
              if (!allowPower || nums[i] >= 10 || nums[j] >= 10) {
                skipOperation = true;
                continue;
              }

              newVal = (k == 4) ? pow(nums[j], nums[i]) : pow(nums[i], nums[j]);
              newFormat = "{}**{}";
              break;
          }

          if (skipOperation) {
            continue;
          }

          vector<int> newNums;

          for (size_t k = 0; k < nums.size(); k++) {
            if (k != i && k != j) {
              newNums.push_back(nums[k]);
            }
          }
          newNums.push_back(newVal);

          auto [formatString, order] =
              solve(newNums, solveDict, orderDict, allowPower);

          if (!formatString.empty()) {
            auto updatedNumsId =
                find(newNums.begin(), newNums.end(), newVal) - newNums.begin();
            auto updatedOrderId =
                find(order.begin(), order.end(), updatedNumsId) - order.begin();

            string updatedFormatString = formatString;
            size_t pos = 0;
            int count = 0;

            while ((pos = updatedFormatString.find("{}", pos)) !=
                   string::npos) {
              if (count == updatedOrderId) {
                updatedFormatString = updatedFormatString.substr(0, pos) + "(" +
                                      newFormat + ")" +
                                      updatedFormatString.substr(pos + 2);
                break;
              }

              count++;
              pos += 2;
            }

            vector<int> vals;

            for (size_t idx = 0; idx < order.size(); idx++) {
              if (idx < updatedOrderId) {
                vals.push_back(newNums[order[idx]]);
              } else if (idx == updatedOrderId) {
                vals.push_back(k < 5 ? nums[j] : nums[i]);
                vals.push_back(k < 5 ? nums[i] : nums[j]);
              } else {
                vals.push_back(newNums[order[idx]]);
              }
            }

            vector<bool> taken(nums.size(), false);
            vector<int> newOrder;

            for (int val : vals) {
              size_t index = 0;

              while (index < nums.size()) {
                if (nums[index] == val && !taken[index]) {
                  taken[index] = true;
                  newOrder.push_back(index);
                  break;
                }
                index++;
              }
            }

            solveDict[key] = updatedFormatString;
            orderDict[key] = newOrder;

            return {updatedFormatString, newOrder};
          }
        }
      }
    }

    return {"", vector<int>()};
  }
}

int main(int argc, char* argv[]) {
  string numStr;
  bool allowPower = false;

  for (int i = 1; i < argc; i++) {
    string arg(argv[i]);

    if (arg == "--allow_powe") {
      allowPower = true;
    } else if (arg == "--num") {
      if (i + 1 < argc) {
        numStr = argv[++i];
      }
    }
  }

  map<string, string> solveDict;
  map<string, vector<int>> orderDict;
  vector<int> nums;

  for (char c : numStr) {
    nums.push_back(c - '0');
  }

  auto [formatString, order] = solve(nums, solveDict, orderDict, allowPower);

  if (!formatString.empty()) {
    string equation = formatString;
    size_t pos = 0;

    for (int o : order) {
      pos = equation.find("{}", pos);
      if (pos != string::npos) {
        equation.replace(pos, 2, to_string(nums[o]));
      }
    }

    cout << equation << endl;
  } else {
    cout << "No answer" << endl;
  }

  return 0;
}
