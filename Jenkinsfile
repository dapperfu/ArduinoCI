node {
   def mvnHome
   stage('Preparation') { // for display purposes
      // Get some code from a GitHub repository
      checkout scm
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
