def arduinos=[["nano", "atmega168"],
              ["nano", "atmega328"],
              ["mega", "atmega1280"],
              ["mega", "atmega2560"],
             ]

def node_factory(project, board, mcu) {
            node {
                stage("Checkout") {
                    git credentialsId: '37739cd2-9654-4774-9380-79e73137d547', url: 'git@github.com:jed-frey/ArduinoCI.git'
                }
                stage('Setup Environment') {
                    sh([script: "make -j2 env"])
                }
                stage("Build ${project}") {
                    withEnv(["BOARD_TAG=${board}", "BOARD_SUB=${mcu}"]) {
                        sh([script: "make ${project}"])
                    }
                }
                stage("Archive Artifacts") {
                    archiveArtifacts([artifacts: '*/build-*/*.elf, */build-*/*.hex'])
                    fingerprint '*/build-*/*.elf, */build-*/*.hex'
                }
                stage("Delete Builds") {
                    sh([script: "rm -rf */build-*"])
                }
            }
}

def projects=["Blink"]
def builds = [:]

for (int j = 0; j < projects.size(); j++) {
    def project = projects.get(j)
    for (int i = 0; i < arduinos.size(); i++) {
        // Get the actual string here.
        def board = arduinos.get(i)[0]
        def mcu = arduinos.get(i)[1]
        // Into each branch we put the pipeline code we want to execute
        builds["${project}_${board}_${mcu}"] = {
            node_factory("${project}", "${board}", "${mcu}")
        }
    }
}

parallel builds
