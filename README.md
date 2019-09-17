# staging-performance-tests

**To run Cheetah:**

cheetah create-campaign -a \<path-to-exe-and-input-files\> -e \<path to spec file\> -m summit -o \<where to create campaign on gpfs\>
 
Once the campaign is created, you will see path-to-campaign/your-username/ . Either run `run-all.sh` here to launch all jobs, or cd into one of the directories and launch `submit.sh`.


**Monitoring a campaign:**

cheetah status \<campaign path\> options
-n for summary
 
**Generate a performance report:**

cheetah generate-report \<campaign path\>
