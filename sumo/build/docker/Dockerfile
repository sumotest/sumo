FROM grgrlmml/sumo_grpc 
ADD setup.sh /root/setup.sh
RUN chmod +x /root/setup.sh
RUN /root/setup.sh
ADD run.sh /run.sh
RUN chmod +x /run.sh
ADD update_jps.sh /update_jps.sh
RUN chmod +x /update_jps.sh
ADD update_sumo.sh /update_sumo.sh
RUN chmod +x /update_sumo.sh
#EXPOSE 9000 
#ENTRYPOINT ["./test.sh"]
#CMD ["/root/update_build_and_run_jpscore.sh"]