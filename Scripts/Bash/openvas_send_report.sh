#!/bin/bash +x

today_date=$(date '+%Y_%m_%d__%H_%M_%S')
task_id="73ca2221-1a25-4a87-97eb-88070ff1507a"
last_report_id=$(omp -u admin -w "your__openvas_password" --get-tasks $task_id  | tail -n 1 | awk '{print $1}')
report_file_path=/home/your_name/openvasreporting-master/openvasreporting/$last_report_id.xml
report_excel_path=/home/your_name/openvas_reports/openvas_$today_date.xlsx
report_xml=$(omp -u admin -w "your__openvas_password" --get-report $last_report_id)
report_email="email@domain"
report_path=/home/your_name/openvasreporting-master/openvasreporting/


echo "$report_xml" > "$report_file_path"
x=$(cd /home/your_name/openvasreporting-master/; python3 -m openvasreporting  -i $report_file_path  -o $report_excel_path -f xlsx)
y=$(cd /home/your_name/openvas_reports/; echo "I scanned stuff" | mail -s "Openvas Scan report of $today_date" -A "openvas_$today_date.xlsx"  $report_email)
