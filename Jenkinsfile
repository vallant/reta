def qualityGate = [[threshold: 1, type: "DELTA", unstable: true]]
def referenceJob =  "bb/pluginreg/develop"

def cmd(command)
{
  if(isUnix())
    return sh(script: command, returnStatus: true)
  return bat(script: command, returnStatus: true)
}

def pipeline = 
{
  def nodeName = isUnix() ? "mac" : "win"
  def cmake = isUnix() ? "/usr/local/bin/cmake" : "C:/ProgramFiles/CMake/bin/cmake.exe"
  def generator = isUnix() ? "Xcode" : "\"Visual Studio 16\""
  stage("${nodeName}: checkout")
  {
    checkout(scm)
  }

  stage("${nodeName}: build")
  {
    dir("build") { deleteDir() }
    dir("build") 
    {
      def status = cmd("${cmake} .. -G ${generator}")
      status += cmd("${cmake} --build . --target reta --config Release --parallel 4")
      status += cmd("${cmake} --build . --target reta_test --config Release --parallel 4")
      
      recordIssues(tool: isUnix() ? clang() : msBuild(), enabledForFailure: true, qualityGates: qualityGate, referenceJobName: referenceJob)
      if(status != 0)
        error()
      archiveArtifacts(isUnix() ? "bin/Release/*.app/**/MacOS/*" : "bin/Release/*.exe")
    }
  }
  
  stage("${nodeName}: test")
  {
    dir("build")
    {
      def status = cmd("${cmake} --build . --target run_tests --config Release")
      status += cmd("${cmake} --build . --target run_analysis --config Release")

      // sometimes files are not written yet, since shell isn't blocking
      sleep(15)

      junit("catch_${nodeName}.xml")
      archiveArtifacts("catch_${nodeName}.xml")
      recordIssues(tool: cppCheck(id: "cppcheck-${nodeName}"), enabledForFailure: true, qualityGates: qualityGate, referenceJobName: referenceJob)
    }
  }
}

node("master")
{
  def pipelines = [:]
  pipelines["mac"] = { node("osx", pipeline) }
  pipelines["win"] = { node("windows-vs2019", pipeline) }
  parallel(pipelines)
}