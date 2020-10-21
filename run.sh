function basic(){
   rm ./myoutput/Summary.diff
   for i in ./tests.fraig/*.aag; do
      name=$(echo $i | sed -e 's/.\/tests.fraig\///g' -e 's/.aag//g')
      echo "Generating" $name".do ..." 
      # generate dofile in mydofile
      # need to add prefix mydofile/ in dofile.js
      node mydofile/dofile.js $i "./mydofile/"$name".do" basic
      # run program
      ./fraig -f "./mydofile/"$name".do" > "myoutput/"$name".out" 2>&1
      echo "Ref starts running" $name".do ..." 
      ./ref/fraig -f "./mydofile/"$name".do" > "myoutput/"$name"-ref.out" 2>&1
      # diff output
      # colordiff (show in terminal)
      colordiff "myoutput/"$name".out" "myoutput/"$name"-ref.out" |tee "myoutput/"$name".diff"
      # generate single diff file && append Summary diff file
      echo "#########"$name"#########" >> "myoutput/Summary.diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" > "myoutput/"$name".diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" >> "myoutput/Summary.diff"
   done
}
function sweep(){
   rm ./myoutput/Summary.diff
   for i in ./tests.fraig/*.aag; do
      name=$(echo $i | sed -e 's/.\/tests.fraig\///g' -e 's/.aag//g')
      echo "Generating" $name".do ..." 
      # generate dofile in mydofile
      # need to add prefix mydofile/ in dofile.js
      node mydofile/dofile.js $i "./mydofile/"$name".do" sweep
      # run program
      ./fraig -f "./mydofile/"$name".do" > "myoutput/"$name".out" 2>&1
      ./ref/fraig -f "./mydofile/"$name".do" > "myoutput/"$name"-ref.out" 2>&1
      # diff output
      # colordiff (show in terminal)
      colordiff "myoutput/"$name".out" "myoutput/"$name"-ref.out" |tee "myoutput/"$name".diff"
      # generate single diff file && append Summary diff file
      echo "#########"$name"#########" >> "myoutput/Summary.diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" > "myoutput/"$name".diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" >> "myoutput/Summary.diff"
   done
}
function opt(){
   rm ./myoutput/Summary.diff
   for i in ./tests.fraig/*.aag; do
      name=$(echo $i | sed -e 's/.\/tests.fraig\///g' -e 's/.aag//g')
      echo "Generating" $name".do ..." 
      # generate dofile in mydofile
      # need to add prefix mydofile/ in dofile.js
      node mydofile/dofile.js $i "./mydofile/"$name".do" opt
      # run program
      ./fraig -f "./mydofile/"$name".do" > "myoutput/"$name".out" 2>&1
      ./ref/fraig -f "./mydofile/"$name".do" > "myoutput/"$name"-ref.out" 2>&1
      # diff output
      # colordiff (show in terminal)
      colordiff "myoutput/"$name".out" "myoutput/"$name"-ref.out" |tee "myoutput/"$name".diff"
      # generate single diff file && append Summary diff file
      echo "#########"$name"#########" >> "myoutput/Summary.diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" > "myoutput/"$name".diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" >> "myoutput/Summary.diff"
   done
}
function strash(){
   rm ./myoutput/Summary.diff
   for i in ./tests.fraig/*.aag; do
      name=$(echo $i | sed -e 's/.\/tests.fraig\///g' -e 's/.aag//g')
      echo "Generating" $name".do ..." 
      # generate dofile in mydofile
      # need to add prefix mydofile/ in dofile.js
      node mydofile/dofile.js $i "./mydofile/"$name".do" strash
      # run program
      ./fraig -f "./mydofile/"$name".do" > "myoutput/"$name".out" 2>&1
      ./ref/fraig -f "./mydofile/"$name".do" > "myoutput/"$name"-ref.out" 2>&1
      # diff output
      # colordiff (show in terminal)
      colordiff "myoutput/"$name".out" "myoutput/"$name"-ref.out" |tee "myoutput/"$name".diff"
      # generate single diff file && append Summary diff file
      echo "#########"$name"#########" >> "myoutput/Summary.diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" > "myoutput/"$name".diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" >> "myoutput/Summary.diff"
   done
}
function sim(){
   rm ./myoutput/Summary.diff
   for i in ./tests.fraig/sim*.aag; do
      name=$(echo $i | sed -e 's/.\/tests.fraig\///g' -e 's/.aag//g')
      echo "Generating" $name".do ..." 
      # generate dofile in mydofile
      # need to add prefix mydofile/ in dofile.js
      node mydofile/dofile.js $i "./mydofile/"$name".do" sim
      # run program
      ./fraig -f "./mydofile/"$name".do" > "myoutput/"$name".out" 2>&1
      # TEST SIMFILE
      perl -pi -e 's/.log/-ref.log/g' ./mydofile/$name".do" 

      ./ref/fraig -f "./mydofile/"$name".do" > "myoutput/"$name"-ref.out" 2>&1

      # diff output
      # colordiff (show in terminal)
      colordiff "myoutput/"$name".out" "myoutput/"$name"-ref.out" |tee "myoutput/"$name".diff"
      # generate single diff file && append Summary diff file
      echo "#########"$name"#########" >> "myoutput/Summary.diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" > "myoutput/"$name".diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" >> "myoutput/Summary.diff"

      # TEST SIMFILE
      diff "myoutput/"$name".log" "myoutput/"$name"-ref.log" >> "myoutput/Summary-log.diff"
   done
}
function fraig(){
   rm ./myoutput/Summary.diff
   for i in ./tests.fraig/*.aag; do
      name=$(echo $i | sed -e 's/.\/tests.fraig\///g' -e 's/.aag//g')
      echo "Generating" $name".do ..." 
      # generate dofile in mydofile
      # need to add prefix mydofile/ in dofile.js
      node mydofile/dofile.js $i "./mydofile/"$name".do" fraig
      # run program
      ./fraig -f "./mydofile/"$name".do" | grep -v -e "Fraig:" -e "SAT:" -e "Total #FEC" -e "Sweeping" -e "Strashing" -e "Simplifying" -e "= Value:"> "myoutput/"$name".out"
      mv ./myoutput/ref.aag ./myoutput/my.aag
      ./ref/fraig -f "./mydofile/"$name".do" | grep -v -e "Fraig:" -e "Proving" -e "Total #FEC" -e "Updating" -e "Sweeping" -e "Strashing" -e "Simplifying" -e "= Value:"> "myoutput/"$name"-ref.out"

      # diff output
      # colordiff (show in terminal)
      colordiff "myoutput/"$name".out" "myoutput/"$name"-ref.out" |tee "myoutput/"$name".diff"
      # generate single diff file && append Summary diff file
      echo "#########"$name"#########" >> "myoutput/Summary.diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" > "myoutput/"$name".diff"
      diff "myoutput/"$name".out" "myoutput/"$name"-ref.out" >> "myoutput/Summary.diff"
      ./ref/fraig -f ./mydofile/runMiter | grep -e "AIG          0" >> "myoutput/Summary.diff"
   done
}

if [ $# -ne 1 ]; then
   echo "Usage $0 <basic|sweep|opt|strash|sim|fraig> "
   exit
fi

./clean.sh
make
case $1 in
   "sweep")
      sweep
      ;;
   "opt")
      opt
      ;;
   "strash")
      strash
      ;;
   "sim")
      sim
      ;;
   "fraig")
      fraig
      ;;
   "basic")
      basic
      ;;
esac