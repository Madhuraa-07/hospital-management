#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <stddef.h>
#include <stdbool.h>
int total_cases = 0, Normal = 0, Emergency = 0;

struct patient_details
{
    int id;
    int discharge;
    int phone_on;
    char doctor[20], gender, f_name[50], l_name[50], depart[25];
    int age;
    struct
    {
        int month_no;
        int day;
        int year;
    } date_of_entry;
    int room_no;
};

struct node
{
    struct patient_details patient;
    struct node *next;
};

int ID = 1000, room[9];
struct node *start = NULL;

void insert_to_file(struct patient_details *patient);
void discharge_from_file(int P_Id);
struct node *insert_details(struct node *start);
struct node *change_details(struct node *start);
struct node *display_details(struct node *start);
void display_list(struct node *start);
void display(struct node *start);
int sort_by_month(struct node *start);
void sort_by_name(struct node *start);
void sort_by_id(struct node *start);
void swap_data(struct node *ptr1, struct node *ptr2);
struct node *discharge(struct node *start,int P_Id);
struct node *load_from_file(struct node *start);
void update_file(struct patient_details *patient);

char d_l[12][50] = {"Sidhart Mukherjee", "Dr Ashish Sabharwal", "Dr Naresh Trehan", "Dr Sudhansu Bhatt", "Dr Anandhi Joshi", "Dr Kadabini Ganguly", "Anand Rao", "Dr Manjula Anagini", "Dr Sheela Reddy", "Dr Ganesh Maharaj", "Dr Deepak Chopra", "Dr Sanjay Sachdeva"};
char depart_l[6][20] = {"NEURO ", "CARDIO", "DERMA ", "OPHTHA", "ENT   ", "GYNEO "};

int main()
{
    int p_id;
    int choice, i;
    start = load_from_file(start);
    printf("***************\n\n");
    printf("******WELCOME TO MMMM HOSPITAL *********\n\n");
    printf("***************\n\n");

    for (i = 0; i < 10; i++)
        room[i] = -1;
    
    while (1)
    {

        total_cases = Normal + Emergency;
        printf("***************\n");
        printf("THE TOTAL NUMBER OF ACTIVE CASES:%d\nTHE TOTAL NUMBER OF NORMAL CONSULTATIONS:%d\nTHE TOTAL NUMBER  OF EMERGENCY CASES:%d\n", total_cases, Normal, Emergency);
        printf("***************\n\n");

        int e;
        printf("1.EMERGENCY\n2.NOT EMERGENCY\n3.EXIT\n");
        printf("Please enter your choicee:");
        scanf("%d", &e);

        if (e == 1)
        {
            printf("proceed to emergency ward\n");
            Emergency += 1;
        }
        else if (e == 2)
        {
            printf("\n1.MAKE A NEW PATIENT ENTRY.\n2.DISPLAY DETAILS OF A PATIENT.\n3.CHANGE THE DETAILS OF A PATIENT\n4.DELETE A PATIENT RECORD\nEnter your choice: ");
            scanf("%d", &choice);
            
            switch (choice)
            {
            case 1:
                start = insert_details(start);
                Normal += 1;
                break;
            case 2:
                start = display_details(start);
                break;
            case 3:
                start = change_details(start);
                break;
            case 4:
                printf("Enter the patient id to be deleted: ");
                scanf("%d", &p_id);
                start = discharge(start,p_id);
                Normal -= 1;
                break;
            default:
                printf("\nPlease enter a valid option.\n");
            }
        }
        else
        {
            printf("EXITING THE APPLICATION !!! THANK YOU FOR USING\n");
            exit(0);
        }
    }
}


bool is_id_present(int target_id) {
    FILE *file = fopen("patient_data.csv", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return false;
    }

    int id;
    while (fscanf(file, "%d,", &id) != EOF) {
        if (id == target_id) {
            fclose(file);
            return true;  // ID is already present
        }
        // Skip the rest of the record
        char c;
        while ((c = fgetc(file)) != EOF && c != '\n');
    }

    fclose(file);
    return false;  // ID is not present
}

void insert_to_file(struct patient_details *patient) {
    FILE *file = fopen("patient_data.csv", "a");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    // Check if the ID is already present in the file
    if (is_id_present(patient->id)) {
        // ID is already present, update the existing record
        update_file(patient);
    } else {
        // ID is not present, insert the new record
        fprintf(file, "%d,%s,%s,%c,%d,%d,%d,%d,%s,%s,%d,%d\n",
                patient->id, patient->f_name, patient->l_name,
                patient->gender, patient->age,
                patient->date_of_entry.month_no, patient->date_of_entry.day,
                patient->date_of_entry.year, patient->depart, patient->doctor,
                patient->room_no, patient->phone_on);
    }

    fclose(file);
}

void update_file(struct patient_details *patient) {
    FILE *file = fopen("patient_data.csv", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    FILE *temp_file = fopen("temp_data.csv", "w");
    if (temp_file == NULL) {
        printf("Error opening temporary file for writing.\n");
        fclose(file);
        return;
    }

    int id;
    char c;
    while (fscanf(file, "%d,", &id) != EOF) {
        if (id == patient->id) {
            // Update the existing record
            fprintf(temp_file, "%d,%s,%s,%c,%d,%d,%d,%d,%s,%s,%d,%d\n",
                    patient->id, patient->f_name, patient->l_name,
                    patient->gender, patient->age,
                    patient->date_of_entry.month_no, patient->date_of_entry.day,
                    patient->date_of_entry.year, patient->depart, patient->doctor,
                    patient->room_no, patient->phone_on);

            // Skip the rest of the existing record
            while ((c = fgetc(file)) != EOF && c != '\n');
        } else {
            // Copy unchanged records to the temporary file
            fprintf(temp_file, "%d,", id);
            while ((c = fgetc(file)) != EOF && c != '\n') {
                fputc(c, temp_file);
            }
        }
    }

    fclose(file);
    fclose(temp_file);

    remove("patient_data.csv");
    rename("temp_data.csv", "patient_data.csv");
}

struct node *load_from_file(struct node *start) {
    FILE *file = fopen("patient_data.csv", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return start;
    }

    while (1) {
        struct node *new_node = (struct node *)malloc(sizeof(struct node));
        if (new_node == NULL) {
            printf("Memory allocation failed.\n");
            fclose(file);
            return start;
        }

        if (fscanf(file, "%d,%49[^,],%49[^,],%c,%d,%d,%d,%d,%24[^,],%19[^,],%d,%d",
                   &new_node->patient.id, new_node->patient.f_name, new_node->patient.l_name,
                   &new_node->patient.gender, &new_node->patient.age,
                   &new_node->patient.date_of_entry.month_no, &new_node->patient.date_of_entry.day,
                   &new_node->patient.date_of_entry.year, new_node->patient.depart, new_node->patient.doctor,
                   &new_node->patient.room_no, &new_node->patient.phone_on) == EOF) {
            free(new_node);
            break;  // End of file
        }

        new_node->next = NULL;

        // Update room status
        if (new_node->patient.room_no > 0 && new_node->patient.room_no <= 10) {
            room[new_node->patient.room_no - 1] = new_node->patient.id;
        } else {
            printf("Invalid room number %d for patient ID %d.\n", new_node->patient.room_no, new_node->patient.id);
            free(new_node);
            continue;
        }

        // Add the new node to the linked list
        if (start == NULL) {
            start = new_node;
        } else {
            struct node *ptr = start;
            while (ptr->next != NULL) {
                ptr = ptr->next;
            }
            ptr->next = new_node;
        }
    }

    fclose(file);
    return start;
}

void discharge_from_file(int P_Id) {
    FILE *file = fopen("patient_data.csv", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    FILE *temp_file = fopen("temp_data.csv", "w");
    if (temp_file == NULL) {
        printf("Error opening temporary file for writing.\n");
        fclose(file);
        return;
    }

    int id, age, month, day, year, room_no, phone_on;
    char f_name[50], l_name[50], gender, depart[25], doctor[20];

    while (fscanf(file, "%d,%[^,],%[^,],%c,%d,%d,%d,%d,%[^,],%[^,],%d,%d\n",
                  &id, f_name, l_name, &gender, &age,
                  &month, &day, &year, depart, doctor,
                  &room_no, &phone_on) != EOF)
    {
        if (id != P_Id) {
            fprintf(temp_file, "%d,%s,%s,%c,%d,%d,%d,%d,%s,%s,%d,%d\n",
                    id, f_name, l_name, gender, age,
                    month, day, year, depart, doctor,
                    room_no, phone_on);
        }
    }

    fclose(file);
    fclose(temp_file);

    remove("patient_data.csv");
    rename("temp_data.csv", "patient_data.csv");
}


struct node *insert_details(struct node *start)
{
    // ... (your existing code above)
	struct node *new_node,*ptr;
	int d,m,yr,ag,yrb,mb,db,flag=0,i=0,roomno=0,d_id,depart_id,phoneno;
	char gend,dis[20],doct[20],f_name[50],l_name[50],temp;
	new_node=(struct node *)malloc(sizeof(struct node));
    
	while(i<10)
	{
		if(room[i]==-1)
		{
			
			flag=1;
		}
		i++;
	} 
	if(flag!=1)
		printf("\nSorry,there are no Rooms available at this time.\n");
	else
	{
		printf("Enter date (DD MM YYYY): ");
        scanf("%d%d%d", &d, &m, &yr);

        // Validate the entered date
        while (d < 1 || d > 31 || m < 1 || m > 12 || yr < 1900) {
            printf("Invalid date. Please enter a valid date (DD MM YYYY): ");
            scanf("%d%d%d", &d, &m, &yr);
        }

		printf("Enter Patient first name and last name: ");
        scanf("%s%s",f_name,l_name);
      
		printf("Enter Patient's age: ");
		scanf("%d",&ag);
		printf("Enter Patient's Gender(only initial): ");
		scanf(" %c",&gend);
		printf("Enter the patient's phone no (10 digits): ");
        scanf("%d", &phoneno);

        // Validate the phone number length
        while (phoneno < 1000000000 || phoneno > 9999999999) {
            printf("Invalid phone number. Please enter a 10-digit phone number: ");
            scanf("%d", &phoneno);
        }

		printf("Enter Department name:\n");
        printf("1.NEUROLOGY\n2.CARDIO\n3.DERMATOLOGY\n4.OPHTHAMOLOGY\n5.ENT\n6.GYNEOLOGY\n");
		printf("Enter the department id: ");
		scanf("%d",&depart_id);
		switch(depart_id)
		{
			case 1:
			printf("1.Dr Sidhart Mukherjee\t2.Dr Ashish Sabharwal\n");
			break;
			case 2:
			printf("3.Dr Naresh Trehan\t4.Dr Sudhansu Bhatt\n");
			break;
			case 3:
			printf("5.Dr Anandhi Joshi\t6.Dr Kadabini Ganguly\n");
			break;
			case 4:
			printf("7.Dr Anand Roa\t8.Dr Manjula Anagini\n");
			break;
			case 5:
			printf("9.Dr Sheela Reddy\t10.Dr Ganesh Maharaj\n");
			break;
			case 6:
			printf("11.Dr Deepak Chopra\t12.Dr Snajay Sachdeva\n ");
			break;
			
		}
		printf("Enter In-charge Doctor ID: ");
		scanf("%d",&d_id);
		printf("\n");
		do
        {
            for (i = 0; i < 10; i++)
            {
                if (room[i] == -1)
                {
                    printf("Room number %d is empty.\n", i + 1);
                }
            }
            printf("Enter the Room Number for the patient: ");
            scanf("%d", &roomno);
            if (roomno > 0 && roomno < 11)
            {
                if (room[roomno - 1] != -1)
                {
                    printf("The Room is occupied. Please select from the given Room Number(s) only\n");
                    // Set flag to continue the loop
                    i = 1;
                }
                else
                {
                    new_node->patient.room_no = roomno;
                    room[roomno - 1] = new_node->patient.id;
                    // Set flag to exit the loop
                    i = 0;
                }
            }
            else
            {
                printf("\n**** Invalid Room Number. Please select again. ****\n");
                // Set flag to continue the loop
                i = 1;
            }
        } while (i);  // Continue the loop until i is 0

		printf("-----------------------------------------------------------------------------------------------------------------------\n\n");
		printf("Your doctor is %s\n",d_l[d_id-1]);
		printf("Patient's Generated ID: %d\n",ID);
		printf("-----------------------------------------------------------------------------------------------------------------------\n\n");
		new_node->patient.id=ID;
		strcpy(new_node->patient.f_name,f_name);
		strcpy(new_node->patient.l_name,l_name);
		
		new_node->patient.date_of_entry.month_no=m;
		new_node->patient.date_of_entry.day=d;
		new_node->patient.date_of_entry.year=yr;
		new_node->patient.age=ag;
        new_node->patient.gender=gend;
        new_node->patient.phone_on=phoneno;
		strcpy(new_node->patient.depart,depart_l[depart_id]);
		strcpy(new_node->patient.doctor,d_l[d_id-1]);
		if(start==NULL)
		{
			new_node->next=NULL;
			start=new_node;
			ID++;
		}
		else
		{
			new_node->next=NULL;
			ptr=start;
			while(ptr->next!=NULL)
			{
				ptr=ptr->next;
			}
			ptr->next=new_node;
			ID++;
		}
	}
    // Add this line to insert the new entry into the file
    insert_to_file(&new_node->patient);

    return start;
}

struct node *display_details(struct node *start)
{
	int choice,i=0,m;
	char n[15];
	struct node *ptr;
	ptr=start;
	if(start==NULL)
	{
		printf("-----------------------------Patient database empty! ---------------------------------\n");
		return NULL;
	}
	else
	{
		while(choice!=5)
		{
			ptr=start;
			printf("\n1.Display by first name \n2.Display by ID \n3.Display for a specific month.\n4.Search patient by first name\n5.Exit\n");
			printf("\nEnter a choice: ");
			scanf("%d",&choice);
			switch(choice)
			{
				case 1:
				sort_by_name(start);
				printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
				printf("ID\t\tName\t\t\t\tDepartment\t\t\t\tDoctor\t\t\t\tRoom No.\t\n");
				printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
				while(ptr!=NULL)
				{
					display_list(ptr);
					ptr=ptr->next;
				}
				break;
				case 2:
				sort_by_id(start);
				printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
				printf("ID\t\tName\t\t\t\tDepartment\t\t\t\tDoctor\t\t\t\tRoom No.\t\n");
				printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
				while(ptr!=NULL)
				{
					display_list(ptr);
					ptr=ptr->next;
				}
				break;
				case 3:
				m=sort_by_month(start);
				if(m==0)
					printf("\nNo Entry available for this month\n ");
				else
				{
					printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
				printf("ID\t\tName\t\t\t\tDepartment\t\t\t\tDoctor\t\t\t\tRoom No.\t\n");
				printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
				}
				while(ptr!=NULL)
					{
						if(ptr->patient.date_of_entry.month_no==m)display_list(ptr);
						ptr=ptr->next;
					}
				break;
				case 4:
				printf("Enter patient's first name: ");
				scanf("%s",n);
				while(ptr!=NULL)
				{
					if(!(strcmp(n,ptr->patient.f_name)))
					{
						display(ptr);
						i++;
					}
					ptr=ptr->next;
				}
				if(i==0)
					printf("Name not found.");
				break;
				case 5:
				printf("\n** Returning to Main Menu ** \n");
				break;
				default:
				printf("Invalid choice. Please enter again.\n");
			}
		}
	}
	sort_by_id(start);
	return start;
}

void display(struct node *start)
{
	 char Discharge[15];
	if(start->patient.discharge==0)
		strcpy(Discharge,"Hospitalised");
	else
		strcpy(Discharge,"Discharged"); 
	printf("\nID: %d\n",start->patient.id);
	printf("Patient Name [FIRST LAST] : %s %s\n",start->patient.f_name,start->patient.l_name);
	printf("Gender: %c\n",start->patient.gender);
	printf("Age: %d years\n",start->patient.age);
	printf("Date of entry: %d/%d/%d\n",start->patient.date_of_entry.day,start->patient.date_of_entry.month_no,start->patient.date_of_entry.year);
	printf("Room Number: %d\n",start->patient.room_no);
	printf("Department: %s\tDoctor: Dr. %s\n",start->patient.depart,start->patient.doctor );
	printf("Phoneno: %d\n",start->patient.phone_on);
}
void display_list(struct node *start)
{
		printf("%4d\t\t%8s %5s\t\t\t%6s\t\t\t\t%s\t\t\t%d\n",start->patient.id,start->patient.f_name,start->patient.l_name,start->patient.depart,start->patient.doctor,start->patient.room_no);
}
int sort_by_month(struct node *start)
{
	struct node *pt1,*pt2;
	int m,flag=1;
	long int d1,d2;
	printf("\nEnter Month number for which you want all patient details: ");
	scanf("%d",&m);
	pt1=start;
	while(pt1!=NULL)
	{
		if(pt1->patient.date_of_entry.month_no==m)
			{
			    flag=0;
			    break;
			}
		pt1=pt1->next;
	}
	pt2=pt1;
	if(flag)
		return 0;
	while(pt1->next!=NULL)
	{
		while(pt1->patient.date_of_entry.month_no!=m)
			pt1=pt1->next;

		d1=pt1->patient.date_of_entry.month_no*50+pt1->patient.date_of_entry.day+pt1->patient.date_of_entry.year*1000;
		while(pt2!=NULL)
		{
		    if(pt2->patient.date_of_entry.month_no==m && pt1!=pt2){
                d2=pt2->patient.date_of_entry.month_no*50+pt2->patient.date_of_entry.day+pt2->patient.date_of_entry.year*1000;
                if((d1>d2) && (pt2->patient.date_of_entry.month_no==m))
                    swap_data(pt1,pt2);
		    }
			pt2=pt2->next;
		}
		pt1=pt1->next;
	}
    return m;
}
void sort_by_id(struct node *start)
{
	struct node *pt1,*pt2;
	pt1=start;
	while(pt1->next!=NULL)
	{
		pt2=pt1->next;
		while(pt2!=NULL)
		{
			if(pt1->patient.id>pt2->patient.id)
				swap_data(pt1,pt2);
			pt2=pt2->next;
		}
		pt1=pt1->next;
	}
}
void sort_by_name(struct node *start)
{
	struct node *pt1,*pt2;
	pt1=start;
	while(pt1->next!=NULL)
	{

		pt2=pt1->next;
		while(pt2!=NULL)
		{
			if(strcmp(pt1->patient.f_name,pt2->patient.f_name)>0)
				swap_data(pt1,pt2);
			pt2=pt2->next;
		}
		pt1=pt1->next;
	}
}
void swap_data(struct node *ptr1, struct node *ptr2){
	struct patient_details temp;
	temp=ptr1->patient;
	ptr1->patient=ptr2->patient;
	ptr2->patient=temp;
}

struct node *change_details(struct node *start)
{
    struct node *ptr;
    int change_id, option, status, roomno, i = 0, flag = 0, phoneno;
    char l_name[20], f_name[20], depart[20], doct[20];
    ptr = (struct node *)malloc(sizeof(struct node));
    ptr = start;

    // ... (your existing code above)
	if(ptr==NULL)
	{
		printf("* No details available!! Please make an entry. *\n");
		return NULL;
	}
	else
	{
		printf("Which patient's detail is to be changed? Please enter the patient's Id: ");
		//discharge_from_file(change_id);
        scanf("%d",&change_id);
		while(ptr!=NULL && ptr->patient.id!=change_id)
		{
			ptr=ptr->next;
		}
		if(ptr==NULL)
		{
			printf("There is no patient with such id.\n");
			free(ptr);
			return start;
		}
		else
		{
			do
			{
				printf("Which detail is to be changed?\n1.Phoneno of the patient\t2.Name\t3.Room Number\t4.Doctor Assigned and deparment \t5.Exit\nEnter your option:");
                discharge_from_file(change_id);
                scanf("%d",&option);
				switch(option)
				{
					case 1:
					printf("Enter the new phone_n: ");
					scanf("%d",&phoneno);
					ptr->patient.phone_on=phoneno;
					break;
					case 2:
					printf("Enter Patient's new first name and last name: ");
					scanf("%s %s",f_name,l_name);
					
					strcpy(ptr->patient.f_name,f_name);
                    strcpy(ptr->patient.l_name,l_name);
					
					break;
					case 3:
					while(i<10)
					{
						if(room[i]==-1)
						{
							printf("Room number %d is empty.\n",i+1);
							flag=1;
						}
						i++;
					}
					if(flag!=1)
						printf("There are no rooms empty at this time.Please try again later.\n");
					else
					{
						do
						{
							printf("Enter new Room Number for the patient:");
							scanf("%d",&roomno);
							if(room[roomno-1]!=-1)
							{
								printf("The room is occupied.Please select from the given room no.s\n");
								 for(i=0;i<10;i++)
								{
									if(room[i]==-1)
									{
										printf("Room number %d is empty.\n",i+1);
										
									}
									
								}
								i=1;
							}
							else
							{
								room[ptr->patient.room_no-1]=-1;
								ptr->patient.room_no=roomno;
								room[roomno-1]=ptr->patient.id;
								i=0;
							}
						}while(i);
					}
					break;
					case 4:
					printf("WHAT DO YOU WANT TO CHANGE:\n1.DEPARTMENT\n2.DOCTOR\n");
					int ch,depart_id;
					scanf("%d",&ch);
					if(ch==1)
					{
						printf("1.NEUROLOGY\n2.CARDIO\n3.DERMATOLOGY\n4.OPHTHAMOLOGY\n5.ENT\n6.GYNECOLOGY\n");
						printf("Enter the new department id: ");
						scanf("%d",&depart_id);
						strcpy(ptr->patient.depart,depart_l[depart_id-1]);
					}
					else{
						char temp[20];
						strcpy(depart,ptr->patient.depart);
						for(int i=0;i<sizeof(depart_l);i++)
						{
							strcpy(temp,depart_l[i]);
							if(strcmp(temp,depart)==0)
							{
								depart_id=i;
							}
						}
						switch(depart_id)
						{
							case 1:
							printf("1.Dr Sidhart Mukherjee\t2.Dr Ashish Sabharwal\n");
							break;
							case 2:
							printf("3.Dr Naresh Trehan\t4.Dr Sudhansu Bhatt\n");
							break;
							case 3:
							printf("5.Dr Anandhi Joshi\t6.Dr Kadabini Ganguly\n");
							break;
							case 4:
							printf("7.Dr Anand Roa\t8.Dr Manjula Anagini\n");
							break;
							case 5:
							printf("9.Dr Sheela Reddy\t10.Dr Ganesh Maharaj\n");
							break;
							case 6:
							printf("11.Dr Deepak Chopra\t12.Dr Snajay Sachdeva\n ");
							break;
							
						}
						int d_id;
						printf("Enter In-charge Doctor ID: ");
						scanf("%d",&d_id);
						strcpy(ptr->patient.doctor,d_l[d_id-1]);
						
					}
					break;
					case 5:
					printf("\n****Returning to Main Menu ***** \n");
					break;
					default:
					printf("Wrong choice\n");
				}
				printf("-------------------------------------DETAILS HAVE BEEN UPDATED-------------------------------------\n");
				return start;

			}while(option!=5);
		}
	}
    // Add this line to update the patient details in the file
    update_file(&ptr->patient);

    return start;
}


    // ... (your existing code above)

struct node *discharge(struct node *start, int P_Id) {
    struct node *ptr = start;
    struct node *prev = NULL;

    while (ptr != NULL) {
        if (ptr->patient.id == P_Id) {
            printf("Record with ID %d Found!\n", P_Id);

            // Remove the patient from the linked list
            if (prev == NULL) {
                // If the patient is at the beginning of the list
                start = ptr->next;
            } else {
                // If the patient is in the middle or end of the list
                prev->next = ptr->next;
            }

            // Free the memory allocated for the discharged patient
            free(ptr);

            // Update room status
            room[P_Id - 1001] = -1;

            printf("Patient with ID %d has been discharged.\n", P_Id);

            // Update total cases
            total_cases--;

            // Remove the discharged patient from the file
            discharge_from_file(P_Id);

            return start;
        }

        prev = ptr;
        ptr = ptr->next;
    }

    printf("Patient with ID %d not found.\n", P_Id);

    return start;

}