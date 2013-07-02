%define _optdir /opt
Name:       liblocation-appman
Summary:    Location DB controler
Version:    0.1.6
Release:    1
Group:      System/Libraries
License:    TBD
Source0:    %{name}-%{version}.tar.gz
Source1001: 	liblocation-appman.manifest

Requires(post): sqlite
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  libgcrypt-devel

%description
Description: Location DB controller library


%package devel
Summary:    Location DB controller (Development files)
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Description : Location DB controller Development Package


%prep
%setup -q -n %{name}-%{version}
cp %{SOURCE1001} .


%build
%cmake .

# Call make instruction with smp support
make %{?jobs:-j%jobs}

%post
/sbin/ldconfig

chown 0:5000 /opt/dbspace/.location-appman.db*
chmod 664 /opt/dbspace/.location-appman.db*
if [ -f %{_libdir}/rpm-plugins/msm.so ]
then
#chsmack -a 'location_fw::db' /opt/dbspace/.location-appman.db*
fi

%postun -p /sbin/ldconfig

%install
rm -rf %{buildroot}
%make_install


%clean
rm -rf %{buildroot}

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/liblocation-appman.so*
%{_optdir}/dbspace/.location-appman.db*

%files devel
%manifest %{name}.manifest
%{_includedir}/location-appman/*.h
%{_libdir}/pkgconfig/*.pc
