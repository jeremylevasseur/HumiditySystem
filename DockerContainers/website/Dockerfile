
From centos:7

#Install epel-release repo
RUN yum -y install --setopt=tsflags=nodocs epel-release && yum -y install --setopt=tsflags=nodocs nginx && yum clean all

EXPOSE 85
EXPOSE 443
#start nginx
CMD ["nginx", "-g", "daemon off;"]

