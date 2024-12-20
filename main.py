import re
import sys


def is_solvable(nums, solvable_dict):
    nums.sort()

    if tuple(nums) in solvable_dict:
        return solvable_dict[tuple(nums)]
    elif len(nums) == 1:
        if nums[0] == 10:
            return True
        else:
            return False
    else:
        for i in range(len(nums) - 1):
            for j in range(i + 1, len(nums)):
                for k in range(4):
                    match k:
                        case 0:
                            new_val = nums[i] + nums[j]
                        case 1:
                            new_val = nums[j] - nums[i]
                        case 2:
                            new_val = nums[i] * nums[j]
                        case 3:
                            if nums[i] == 0 or nums[j] % nums[i] != 0:
                                continue

                            new_val = nums[j] / nums[i]

                    new_nums = [*nums[:i], *nums[i + 1 : j], *nums[j + 1 :], new_val]
                    solvable_dict[tuple(new_nums)] = is_solvable(
                        new_nums, solvable_dict
                    )

                    if solvable_dict[tuple(new_nums)]:
                        return True
        return False


def solve(nums, solve_dict, order_dict):
    nums.sort()

    if tuple(nums) in solve_dict:
        return solve_dict[tuple(nums)], order_dict[tuple(nums)]
    elif len(nums) == 1:
        if nums[0] == 10:
            return "{}", [0]
        else:
            return "", []
    else:
        for i in range(len(nums) - 1):
            for j in range(i + 1, len(nums)):
                for k in range(4):
                    match k:
                        case 0:
                            new_val = nums[j] + nums[i]
                            new_format = "{}+{}"
                        case 1:
                            new_val = nums[j] - nums[i]
                            new_format = "{}-{}"
                        case 2:
                            new_val = nums[j] * nums[i]
                            new_format = "{}*{}"
                        case 3:
                            if nums[i] == 0 or nums[j] % nums[i] != 0:
                                continue

                            new_val = int(nums[j] / nums[i])
                            new_format = "{}/{}"

                    new_nums = [*nums[:i], *nums[i + 1 : j], *nums[j + 1 :], new_val]
                    format_string, orders = solve(new_nums, solve_dict, order_dict)

                    if len(format_string) > 0:
                        updated_nums_id = new_nums.index(new_val)
                        updated_orders_id = orders.index(updated_nums_id)
                        updated_format_id = format_string.replace(
                            "{}", "XX", updated_orders_id
                        ).find("{}")

                        format_string = (
                            format_string[:updated_format_id]
                            + "("
                            + new_format
                            + ")"
                            + format_string[updated_format_id + 2 :]
                        )

                        vals = [
                            *[new_nums[order] for order in orders[:updated_orders_id]],
                            nums[j],
                            nums[i],
                            *[
                                new_nums[order]
                                for order in orders[updated_orders_id + 1 :]
                            ],
                        ]

                        taken_dict = {}
                        orders = []

                        for val in vals:
                            index = nums.index(val)

                            while True:
                                if index in taken_dict:
                                    index += 1
                                    continue

                                break

                            taken_dict[index] = True
                            orders.append(index)

                        assert sum(orders) == sum(range(len(nums)))

                    solve_dict[tuple(nums)] = format_string
                    order_dict[tuple(nums)] = orders

                    if len(format_string) > 0:
                        return format_string, orders
        return "", []


translate_dict = {
    r"(^|\+|\()\(([{}+-]+){}\)($|\+|-|\))": r"\1\2{}\3",
    r"\({}\*{}\)": r"{}*{}",
    r"-\(([^()]*?)\)": lambda match: f'-{match.group(1).replace("+", "TEMP").replace("-", "+").replace("TEMP", "-")}',
}


def clean_format_string(format_string):
    while True:
        new_format_string = format_string

        for old, new in translate_dict.items():
            new_format_string = re.sub(old, new, new_format_string)

        if new_format_string != format_string:
            format_string = new_format_string
            continue

        return new_format_string


def main():
    num = sys.argv[1]
    nums = [int(n) for n in str(num)]
    nums.sort()

    solve_dict = {}
    order_dict = {}

    format_string, orders = solve(nums, solve_dict, order_dict)
    format_string = clean_format_string(format_string[1:-1])
    equation = format_string.format(*[nums[order] for order in orders])
    
    if len(equation) > 0:
        print(equation)
        assert eval(equation) == 10, "Equation corrupted"
    else:
        print("No answer")


if __name__ == "__main__":
    main()
