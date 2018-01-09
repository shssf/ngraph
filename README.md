# ngraph-neon

## Installation

Follow these steps to install the ngraph's python wrapper and its prerequisites.

```
### private-ngraph-cpp

Download the required version of private-ngraph-cpp and install it.
```
git clone https://github.com/NervanaSystems/private-ngraph-cpp.git
cd private-ngraph-cpp
git checkout 2f0a262e359796e0848804d4f5b00ae9a99970dc -b "local branch name"
Build and Install it : https://github.com/NervanaSystems/private-ngraph-cpp#steps
```

### ngraph-neon

After installing private-ngraph-cpp, follow the steps below to install ngraph-neon.
The NGRAPH_CPP_BUILD_PATH is set to default installation location of private-ngraph-cpp.
```
git clone --recursive https://github.com/NervanaSystems/ngraph-neon.git
cd ngraph-neon
export NGRAPH_CPP_BUILD_PATH=$HOME/ngraph_dist/
pip install -U .
```
