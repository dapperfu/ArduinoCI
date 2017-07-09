node {
   def mvnHome
   stage('Preparation') { // for display purposes
      // Get some code from a GitHub repository
      git credentialsId: '37739cd2-9654-4774-9380-79e73137d547', url: 'git@github.com:jed-frey/ArduinoCI.git'
   }
   stage('Setup Environment') {
      sh "make env"
   }
   stage('Build Blink') {
      withEnv(["BOARD_SUB=atmega168"]) {
          sh "make Blink"
      }      
      withEnv(["BOARD_SUB=atmega328"]) {
          sh "make Blink"
      }
      archiveArtifacts '*/build-*/*.elf, */build-*/*.hex'
   }
}
