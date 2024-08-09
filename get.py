import requests
from concurrent.futures import ThreadPoolExecutor, as_completed


def make_request():
    response = requests.get('http://localhost:8000')
    return response.status_code

def main():
    num_requests = 25000
    num_threads = 100  # Adjust this based on your system capabilities

    with ThreadPoolExecutor(max_workers=num_threads) as executor:
        futures = [executor.submit(make_request) for _ in range(num_requests)]

        for future in as_completed(futures):
            try:
                status_code = future.result()
                print(f"Request completed with status code: {status_code}")
            except Exception as e:
                print(f"Request generated an exception: {e}")

if __name__ == "__main__":
    main()
