import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', type=str, dest="input")

    args = parser.parse_args()

    print(args.input)

if __name__ == "__main__":
    main()