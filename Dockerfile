FROM ced2911/saturntoolchain:latest

# fix vscode formatter
RUN apt update && apt install xxd zlib1g-dev libtinfo5 -y
