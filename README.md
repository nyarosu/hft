<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/github_username/repo_name">
    <img src="https://miro.medium.com/v2/resize:fit:1358/1*795TCPCY9EnT8lB7j49MZg.jpeg" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">A high frequency trading system built with C++</h3>

  <p align="center">
    A fast, high performance, low latency automated trading system built using C++, C and some x86 assembly.
    <br />
    <a href=""><strong>Explore the docs »</strong></a>
    <br />
    <br />
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](https://example.com)
A high performance, low latency high frequency trading system written in C++. Utilizes custom data structures, memory allocators, and several low latency techniques to execute a large amount of concurrent trades, each with a minimal amount of latency.
Built for Linux x86 systems only (some inline x86 ASM is used, and some POSIX system calls and APIs are used too). You can try to run it on WSL if you are on x86 Windows, but I've not tested this.
<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With

* C++ (vast majority)
* C (only for certain system calls/APIs)
* x86 assembly (certain inline portions)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

Build with cmake, run tests with ctest to test if everything works. If all good, set env vars and run the binary.

### Prerequisites

This is an example of how to list things you need to use the software and how to install them.
* An x86 based, Linux system. The project will only build and run on this, as I mentioned before due to proprietary system calls and inline ASM. I will try to make this project more portable in the future if I can.
* cmake
  ```sh
  sudo apt-get install cmake
  ```
* C++20 capable compiler (use GCC for maximal compatibility). This should be pre installed on Ubuntu, but if not, find the instructions to install it for your distro. 

### Installation

1. TODO: steps on setting up a trading account and getting/setting API keys/credentials, haven't decided on a provider yet.
2. Clone the repo
   ```sh
   git clone https://github.com/nyarosu/hft.git
   ```
3. Build with cmake. 
   ```sh
   cd build && cmake -S .. -B . && cmake --build .
   ```
4. Run tests to make sure everything works (be inside build, after running the prior commands)
   ```sh
   ctest
   ```
5. Start the trading system (from inside build)
   ```sh
   ./app/HFT
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>









<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>




<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/github_username/repo_name.svg?style=for-the-badge
[contributors-url]: https://github.com/github_username/repo_name/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/github_username/repo_name.svg?style=for-the-badge
[forks-url]: https://github.com/github_username/repo_name/network/members
[stars-shield]: https://img.shields.io/github/stars/github_username/repo_name.svg?style=for-the-badge
[stars-url]: https://github.com/github_username/repo_name/stargazers
[issues-shield]: https://img.shields.io/github/issues/github_username/repo_name.svg?style=for-the-badge
[issues-url]: https://github.com/github_username/repo_name/issues
[license-shield]: https://img.shields.io/github/license/github_username/repo_name.svg?style=for-the-badge
[license-url]: https://github.com/github_username/repo_name/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username
[product-screenshot]: images/screenshot.png
[Next.js]: https://img.shields.io/badge/next.js-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/
[Vue.js]: https://img.shields.io/badge/Vue.js-35495E?style=for-the-badge&logo=vuedotjs&logoColor=4FC08D
[Vue-url]: https://vuejs.org/
[Angular.io]: https://img.shields.io/badge/Angular-DD0031?style=for-the-badge&logo=angular&logoColor=white
[Angular-url]: https://angular.io/
[Svelte.dev]: https://img.shields.io/badge/Svelte-4A4A55?style=for-the-badge&logo=svelte&logoColor=FF3E00
[Svelte-url]: https://svelte.dev/
[Laravel.com]: https://img.shields.io/badge/Laravel-FF2D20?style=for-the-badge&logo=laravel&logoColor=white
[Laravel-url]: https://laravel.com
[Bootstrap.com]: https://img.shields.io/badge/Bootstrap-563D7C?style=for-the-badge&logo=bootstrap&logoColor=white
[Bootstrap-url]: https://getbootstrap.com
[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com 
